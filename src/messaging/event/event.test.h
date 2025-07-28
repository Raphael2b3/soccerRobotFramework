#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"

;

DEFINE_AGENT(DummyAgent){
};

struct DummyEvent :  Event<DummyEvent> {
    std::string message;
    DummyEvent() = default;
    explicit DummyEvent(std::string msg) : message(std::move(msg)) {}
};


// ---------------------
//        TESTS
// ---------------------

TEST_CASE("BaseEvent equality operator and constructors") {
    AgentId id1 = AgentId::getNewId("test");
    priority_t p = 5;

    BaseEvent e1(id1, p);
    BaseEvent e2(id1, p);

    CHECK(e1.sender_id == id1);
    CHECK(e1.priority == p);
    CHECK_FALSE(e1 == nullptr);
    CHECK(e1 == &e1);          // pointer comparison
    CHECK_FALSE(e1 == &e2);    // different address
}

TEST_CASE("Event<T> subscribe, emit, unsubscribe") {
    auto agent = DummyAgent::spawnNewAgent();

    // Subscribe
    DummyEvent::subscribe<InMemoryBackend>(agent);
    CHECK(agent->mailbox.last_event == nullptr);

    // Emit
    auto evt = std::make_shared<DummyEvent>("hello");
    DummyEvent::emit<InMemoryBackend>(evt);
    CHECK(agent->mailbox.last_event != nullptr);
    auto received = std::dynamic_pointer_cast<DummyEvent>(agent->mailbox.last_event);
    CHECK(received != nullptr);
    CHECK(received->message == "hello");

    // Unsubscribe and emit again
    DummyEvent::unsubscribe(agent);
    agent->mailbox.last_event = nullptr; // reset
    DummyEvent::emit<InMemoryBackend>(std::make_shared<DummyEvent>("bye"));
    CHECK(agent->mailbox.last_event == nullptr); // no delivery after unsub
}