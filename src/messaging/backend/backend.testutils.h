#ifndef BACKEND_TEST_H
#define BACKEND_TEST_H

#include "agent/agent.h"
#include "messaging/event/event.h"
#include <chrono>
#include <atomic>
#include <vector>
#include <thread>
#include <numeric>
#include <iostream>

constexpr int messages_send = 40;
constexpr int benchmark_agents = 80;


// Benchmark Runner
template <typename Backend>
void backend_benchmark(int N_agents = benchmark_agents) {
    struct BenchmarkValues {
        std::chrono::milliseconds send_duration{};
        std::chrono::milliseconds receive_duration{};
        std::vector<std::chrono::milliseconds> latencies;
    };

    // Event mit Timestamp
    struct BenchmarkEvent : Event<BenchmarkEvent> {
        std::chrono::high_resolution_clock::time_point send_time;
        int sequence = 0;
    };

    // Benchmark-Agent
    DEFINE_AGENT(BenchmarkAgent) {
        BenchmarkValues values;
        std::atomic<int> received_count = 0;

        void init() override {
            on<BenchmarkEvent, Backend>([this](std::shared_ptr<BenchmarkEvent> event) {
                auto now = std::chrono::high_resolution_clock::now();
                auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(now - event->send_time);
                values.latencies.push_back(latency);
                ++received_count;
            });
        }

        void main() override {
            auto send_start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < messages_send; ++i) {
                auto event = std::make_shared<BenchmarkEvent>();
                event->send_time = std::chrono::high_resolution_clock::now();
                event->sequence = i;
                emit<BenchmarkEvent,Backend>(event);
            }

            auto send_end = std::chrono::high_resolution_clock::now();
            values.send_duration = std::chrono::duration_cast<std::chrono::milliseconds>(send_end - send_start);

            // Warten auf Empfang aller Nachrichten (max. 2 Sekunden Timeout)
            auto wait_start = std::chrono::high_resolution_clock::now();
            while (received_count < messages_send) {
                auto now = std::chrono::high_resolution_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - wait_start).count() > 2) {
                    break;
                }
            }
            auto wait_end = std::chrono::high_resolution_clock::now();
            values.receive_duration = std::chrono::duration_cast<std::chrono::milliseconds>(wait_end - wait_start);
        }
    };

    std::vector<std::shared_ptr<BenchmarkAgent> > agents;
    agents.reserve(N_agents);

    auto global_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N_agents; ++i) {
        auto agent = BenchmarkAgent::spawnNewAgent();
        if (agent == nullptr) {
            throw std::runtime_error("Failed to spawn agent");
        }
        agents.push_back(agent);
    }

    // Stoppe alle Agents
    for (auto &agent: agents) {
        agent->kill();
    }

    auto global_end = std::chrono::high_resolution_clock::now();
    auto total_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(global_end - global_start);

    // Auswertung
    std::chrono::milliseconds total_send{};
    std::chrono::milliseconds total_receive{};
    std::vector<std::chrono::milliseconds> all_latencies;

    for (auto &agent: agents) {
        total_send += agent->values.send_duration;
        total_receive += agent->values.receive_duration;
        all_latencies.insert(all_latencies.end(),
                             agent->values.latencies.begin(),
                             agent->values.latencies.end());
    }

    double avg_latency = all_latencies.empty()
                             ? 0.0
                             : std::accumulate(all_latencies.begin(), all_latencies.end(), 0.0,
                                               [](double sum, auto ms) {
                                                   return sum + ms.count();
                                               }) / all_latencies.size();

    std::cout << "Benchmark Results:\n"
            << "Agents: " << N_agents << "\n"
            << "Messages per agent: " << messages_send << "\n"
            << "Total runtime: " << total_runtime.count() << " ms\n"
            << "Avg send duration: " << total_send.count() / N_agents << " ms\n"
            << "Avg receive wait: " << total_receive.count() / N_agents << " ms\n"
            << "Avg message latency: " << avg_latency << " ms\n";
}

#endif // BACKEND_TEST_H
