#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"

class MyEvent : public Event<MyEvent> {
public:
    int antwort_auf_alles = 0;
};

DEFINE_AGENT(InMemoryTestAgent) {
public:
    int *antwort_auf_alles_ptr;

    void init() override {
        on<MyEvent>([this](auto event) {
            *this->antwort_auf_alles_ptr = event->antwort_auf_alles;
        });
    }

    void main() override {
        auto event = std::make_shared<MyEvent>();
        event->antwort_auf_alles = 42;
        emit<MyEvent, InMemoryBackend>(event);
    }
};

TEST_CASE("In Memory Backend Should Work") {
    auto agent = InMemoryTestAgent::spawnNewAgent();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CHECK(*agent->antwort_auf_alles_ptr == 42);

}
