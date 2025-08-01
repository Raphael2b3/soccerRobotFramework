#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"

struct MyEvent : public Event<MyEvent> {
    int antwort_auf_alles = 0;
};



TEST_CASE("In Memory Backend Should Work using on<MyEvent,InMemoryBackend>(..)") {
    DEFINE_AGENT(InMemoryTestAgent) {
        public:
        int antwort_auf_alles = 0;

        void init() override {
            on<MyEvent, InMemoryBackend>([this](auto event) {
                this->antwort_auf_alles = event->antwort_auf_alles;
            });
        }

        void main() override {
            auto event = std::make_shared<MyEvent>();
            event->antwort_auf_alles = 42;
            emit<MyEvent, InMemoryBackend>(event);
        }
    };

    auto agent = InMemoryTestAgent::spawnNewAgent();
    while (agent->antwort_auf_alles != 42){}
    CHECK(agent->antwort_auf_alles == 42);
    agent->kill();
}

TEST_CASE("Benchmark Delay emit -> receive using InMemoryBackend") { // TODO Benchmark Backends

}