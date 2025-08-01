#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"


DEFINE_AGENT(EventTestAgent)
{
    DEFINE_POOL_SIZE(30);
};

struct EventTestEvent : Event<EventTestEvent>
{
    std::string message;
    EventTestEvent() = default;

    explicit EventTestEvent(std::string msg) : message(std::move(msg))
    {
    }
};

class TestBackend
{
public:
    inline static bool initialized = false;
    inline static bool emitted = false;

    template <typename T>
    static void init()
    {
        initialized = true;
    }

    template <typename T>
    static void emit(std::shared_ptr<T> event)
    {
        emitted = true;
        T::template dispatch_event<TestBackend>(event);
    }

    static void reset()
    {
        initialized = false;
        emitted = false;
    }
};

class TestBackend2
{
public:
    inline static bool initialized = false;
    inline static bool emitted = false;

    template <typename T>
    static void init()
    {
        initialized = true;
    }

    template <typename T>
    static void emit(std::shared_ptr<T> event)
    {
        emitted = true;
        T::template dispatch_event<TestBackend2>(event);
    }

    static void reset()
    {
        initialized = false;
        emitted = false;
    }
};

class TestBackend3
{
public:
    inline static bool initialized = false;
    inline static bool emitted = false;

    template <typename T>
    static void init()
    {
        initialized = true;
    }

    template <typename T>
    static void emit(std::shared_ptr<T> event)
    {
        emitted = true;
        T::template dispatch_event<TestBackend3>(event);
    }

    static void reset()
    {
        initialized = false;
        emitted = false;
    }
};

// ---------------------
//        TESTS
// ---------------------

TEST_CASE("Subscribing to an event should initialize the backend")
{
    auto agent = EventTestAgent::spawnNewAgent();
    CHECK(agent!=nullptr);
    TestBackend::reset();
    EventTestEvent::subscribe<TestBackend>(agent);
    CHECK(TestBackend::initialized == true);
}


TEST_CASE("Subscribing to an event via multiple backends should initialize each backend seperately")
{
    auto agent = EventTestAgent::spawnNewAgent();
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    EventTestEvent::subscribe<TestBackend>(agent);
    CHECK(TestBackend::initialized == true);
    CHECK_FALSE(TestBackend2::initialized);
    EventTestEvent::subscribe<TestBackend2, TestBackend3>(agent);
    CHECK(TestBackend3::initialized == true);
    CHECK(TestBackend2::initialized == true);
}

TEST_CASE("Emit will call emit on the backend")
{
    auto event = std::make_shared<EventTestEvent>("Hello World");
    auto agent = EventTestAgent::spawnNewAgent();
    TestBackend::reset();
    EventTestEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
}

TEST_CASE("Subscribing to an event via multiple backends should initialize each backend seperately")
{
    auto event = std::make_shared<EventTestEvent>("Hello World");
    auto agent = EventTestAgent::spawnNewAgent();
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    EventTestEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
    CHECK_FALSE(TestBackend2::emitted);
    EventTestEvent::emit<TestBackend2, TestBackend3>(event);
    CHECK(TestBackend3::emitted == true);
    CHECK(TestBackend2::emitted == true);
}

TEST_CASE("Subscribing an agent to an event and internally emitting an event should call the agents mailbox")
{
    bool event_called = false;
    auto event = std::make_shared<EventTestEvent>("Hello World");
    auto agent = EventTestAgent::spawnNewAgent();
    agent->on<EventTestEvent>([&event_called](std::shared_ptr<EventTestEvent> e)
    {
        CHECK(e->message == "Hello World");
        event_called = true;
    });
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    EventTestEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
    CHECK_FALSE(TestBackend2::emitted);
    EventTestEvent::emit<TestBackend2, TestBackend3>(event);
    CHECK(TestBackend3::emitted == true);
    CHECK(TestBackend2::emitted == true);
}


