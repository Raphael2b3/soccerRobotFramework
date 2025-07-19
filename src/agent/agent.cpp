#include "./agent.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>

Agent::Agent() {
        boost::lock_guard<boost::mutex> lock(agent_mutex);
        runtime_id = id_counter++;
        agent_pool[runtime_id] = this;

        // Start main thread
        main_thread = boost::thread(&Agent::main, this);

        // Start "other stuff" thread (loops forever)
        mailbox_handler_thread = boost::thread([this]() {
            while (true) {
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
                printf("Agent %d is doing other stuff...\n", runtime_id);
            }
        });
    }

Agent::~Agent() {
    boost::lock_guard<boost::mutex> lock(agent_mutex);
    agent_pool.erase(runtime_id);
}

void Agent::main() {
        //printf("Main function of Agent %d is not implemented \n", runtime_id);
    }

bool Agent::kill() {
    // Implement kill logic if needed
    mailbox_handler_thread.interrupt();
    main_thread.interrupt();
    return true;
}

Agent* Agent::spawnNewAgent() {
    if (agent_pool.size() >= 100) {
        printf("Agent pool is full, cannot spawn new agent.\n");
        return nullptr; // Pool is full
    }

    return new Agent();  // caller responsible for managing memory
}

Agent* Agent::getAgent(agent_id_t id) {
    boost::lock_guard<boost::mutex> lock(agent_mutex);
    auto it = agent_pool.find(id);
    if (it != agent_pool.end()) {
        return it->second;
    }
    return nullptr;
}
