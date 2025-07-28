#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"

struct MyEvent : public Event<MyEvent> {
    int antwort_auf_alles = 0;
};

DEFINE_AGENT(InMemoryTestAgent) {
public:
    int antwort_auf_alles = 0;

    void init() override {

        on<MyEvent>([this](auto event) {
            printf("\nEvent recieved\n");
            this->antwort_auf_alles = event->antwort_auf_alles;
        });
        printf("\nEvent subscribed\n");
    }

    void main() override {
        auto event = std::make_shared<MyEvent>();
        event->antwort_auf_alles = 42;
        emit<MyEvent, InMemoryBackend>(event);
        printf("\nEvent emitted\n");
    }
};

TEST_CASE("In Memory Backend Should Work") {
    auto agent = InMemoryTestAgent::spawnNewAgent();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    agent->kill();
    CHECK(agent->antwort_auf_alles == 42);

}
