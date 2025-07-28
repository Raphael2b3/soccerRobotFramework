#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"

;

DEFINE_AGENT(DummyAgent)
{
    DEFINE_POOL_SIZE(30);
};

struct DummyEvent : Event<DummyEvent>
{
    std::string message;
    DummyEvent() = default;

    explicit DummyEvent(std::string msg) : message(std::move(msg))
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
        T::internal_emit(event);
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
        T::internal_emit(event);
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
        T::internal_emit(event);
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
    auto agent = DummyAgent::spawnNewAgent();
    CHECK(agent!=nullptr);
    TestBackend::reset();
    DummyEvent::subscribe<TestBackend>(agent);
    CHECK(TestBackend::initialized == true);
}


TEST_CASE("Subscribing to an event via multiple backends should initialize each backend seperately")
{
    auto agent = DummyAgent::spawnNewAgent();
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    DummyEvent::subscribe<TestBackend>(agent);
    CHECK(TestBackend::initialized == true);
    CHECK_FALSE(TestBackend2::initialized);
    DummyEvent::subscribe<TestBackend2, TestBackend3>(agent);
    CHECK(TestBackend3::initialized == true);
    CHECK(TestBackend2::initialized == true);
}

TEST_CASE("Emit will call emit on the backend")
{
    auto event = std::make_shared<DummyEvent>("Hello World");
    auto agent = DummyAgent::spawnNewAgent();
    TestBackend::reset();
    DummyEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
}

TEST_CASE("Subscribing to an event via multiple backends should initialize each backend seperately")
{
    auto event = std::make_shared<DummyEvent>("Hello World");
    auto agent = DummyAgent::spawnNewAgent();
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    DummyEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
    CHECK_FALSE(TestBackend2::emitted);
    DummyEvent::emit<TestBackend2, TestBackend3>(event);
    CHECK(TestBackend3::emitted == true);
    CHECK(TestBackend2::emitted == true);
}

TEST_CASE("Subscribing an agent to an event and internally emitting an event should call the agents mailbox")
{
    bool event_called = false;
    auto event = std::make_shared<DummyEvent>("Hello World");
    auto agent = DummyAgent::spawnNewAgent();
    agent->on<DummyEvent>([&event_called](std::shared_ptr<DummyEvent> e)
    {
        CHECK(e->message == "Hello World");
        event_called = true;
    });
    TestBackend::reset();
    TestBackend2::reset();
    TestBackend3::reset();
    DummyEvent::emit<TestBackend>(event);
    CHECK(TestBackend::emitted==true);
    CHECK_FALSE(TestBackend2::emitted);
    DummyEvent::emit<TestBackend2, TestBackend3>(event);
    CHECK(TestBackend3::emitted == true);
    CHECK(TestBackend2::emitted == true);
}


TEST_CASE(
    "subscribing and emiting an event should not Require a Backend to be specified so on<MyEvent>() and emit<MyEvent> is valid")
{
    struct MyEvent : public Event<MyEvent>
    {
        int antwort_auf_alles = 0;
    };
    DEFINE_AGENT(InMemoryTestAgent)
    {
    public:
        int antwort_auf_alles = 0;

        void init() override
        {
            on<MyEvent>([this](auto event)
            {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                this->antwort_auf_alles = event->antwort_auf_alles;
            });
        }

        void main() override
        {
            auto event = std::make_shared<MyEvent>();
            event->antwort_auf_alles = 42;
            emit<MyEvent>(event);
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // wait for the async threads to start
    agent->kill();
    CHECK(agent->antwort_auf_alles == 42);
}

TEST_CASE("using this_from_shared_ptr in on<MyEvent>() should work")
{
    struct MyEvent : public Event<MyEvent>
    {
        int antwort_auf_alles = 0;
    };
    DEFINE_AGENT(InMemoryTestAgent)
    {
    public:
        int antwort_auf_alles = 0;

        void init() override
        {
            on<MyEvent>([this_ptr = shared_from_this()](auto event)
            {
                assert(this_ptr != nullptr && "this should not be null in on<MyEvent>()");
                this_ptr->antwort_auf_alles = event->antwort_auf_alles;
            });
        }

        void main() override
        {
            auto event = std::make_shared<MyEvent>();
            event->antwort_auf_alles = 42;
            emit<MyEvent>(event);
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // wait for the async threads to start
    // TODO avoid sleeping in tests
    agent->kill();
    CHECK(agent->antwort_auf_alles == 42);
}
