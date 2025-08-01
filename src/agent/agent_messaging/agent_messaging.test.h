#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/backend/in_memory/in_memory_backend.h"
#include "messaging/event/event.h"
#include "agent/agent.h"


DEFINE_AGENT(DummyAgent) {
    DEFINE_POOL_SIZE(30);
};

struct DummyEvent : Event<DummyEvent> {
    std::string message;

    DummyEvent() = default;

    explicit DummyEvent(std::string msg) : message(std::move(msg)) {
    }
};


TEST_CASE(
    "subscribing and emiting an event should not Require a Backend to be specified so on<MyEvent>() and emit<MyEvent> is valid") {
    struct MyEvent : Event<MyEvent> {
        int antwort_auf_alles = 0;
    };
    DEFINE_AGENT(InMemoryTestAgent) {
        int antwort_auf_alles = 0;

        void init() override {
            on<MyEvent>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                this->antwort_auf_alles = event->antwort_auf_alles;
            });
        }

        void main() override {
            auto event = std::make_shared<MyEvent>();
            event->antwort_auf_alles = 42;
            emit<MyEvent>(event);
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();

    while (agent->antwort_auf_alles != 42) {
    }
    CHECK(agent->antwort_auf_alles == 42);
}

TEST_CASE("using this_from_shared_ptr in on<MyEvent>() should work") {
    struct MyEvent : public Event<MyEvent> {
        int antwort_auf_alles = 0;
    };
    DEFINE_AGENT(InMemoryTestAgent) {
        int antwort_auf_alles = 0;

        void init() override {
            on<MyEvent>([this_ptr = shared_from_this()](auto event) {
                assert(this_ptr != nullptr && "this should not be null in on<MyEvent>()");
                this_ptr->antwort_auf_alles = event->antwort_auf_alles;
            });
        }

        void main() override {
            auto event = std::make_shared<MyEvent>();
            event->antwort_auf_alles = 42;
            emit<MyEvent>(event);
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();
    while (agent->antwort_auf_alles != 42) {
    }
    CHECK(agent->antwort_auf_alles == 42);
}

TEST_CASE(
    "It should be the same using DEFAULT_BACKEND or not specifying a backend in on<MyEvent>()") {
    struct MyEvent : public Event<MyEvent> {
    };
    DEFINE_AGENT(InMemoryTestAgent) {
        int counter = 0;

        void init() override {
            on<MyEvent>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                counter++;
            });
            on<MyEvent,DEFAULT_BACKEND>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                counter++;
            });
        }

        void main() override {
            auto event = std::make_shared<MyEvent>();
            emit<MyEvent>(event);
            emit<MyEvent,DEFAULT_BACKEND>(event);
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();
    while (agent->counter != 4){}
    CHECK(agent->counter == 4);
}

class TestMessaginBackend {
public:
    template<typename T>
    static void init() {
    }

    template<typename T>
    static void emit(std::shared_ptr<T> event) {
        T::template dispatch_event<TestMessaginBackend>(event);
    }
};

class TestMessaginBackend2 {
public:
    template<typename T>
    static void init() {
    }

    template<typename T>
    static void emit(std::shared_ptr<T> event) {
        T::template dispatch_event<TestMessaginBackend2>(event);
    }
};

class TestMessaginBackend3 {
public:
    template<typename T>
    static void init() {
    }

    template<typename T>
    static void emit(std::shared_ptr<T> event) {
        T::template dispatch_event<TestMessaginBackend3>(event);
    }
};


TEST_CASE(
    "specifying multiple backends should work and they should be treated like different channels") {
    struct MyEvent : public Event<MyEvent> {
        int data = 0;
    };
    DEFINE_AGENT(InMemoryTestAgent){
        int a = 0;
        int b = 0;
        int c = 0;
        int d = 0;

        void init() override {
            on<MyEvent, TestMessaginBackend>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                a++;
            });

            on<MyEvent, TestMessaginBackend2>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                b++;
            });
            on<MyEvent, TestMessaginBackend3>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                c++;
            });

            on<MyEvent, TestMessaginBackend, TestMessaginBackend2, TestMessaginBackend3>([this](auto event) {
                assert(this != nullptr && "this should not be null in on<MyEvent>()");
                d++;
            });
        }

        void main() override {
            auto event = std::make_shared<MyEvent>();
            emit<MyEvent, TestMessaginBackend>(event); // a = 1 d = 1
            emit<MyEvent, TestMessaginBackend2>(event); // b = 1 d = 2
            emit<MyEvent, TestMessaginBackend3>(event); // c = 1 d = 3
            emit<MyEvent, TestMessaginBackend, TestMessaginBackend2, TestMessaginBackend3>(event);
            // a = 2 b = 2 c = 2 d = 6
        }
    };
    auto agent = InMemoryTestAgent::spawnNewAgent();
    CHECK(agent->a == 2);
    CHECK(agent->b == 2);
    CHECK(agent->c == 2);
    CHECK(agent->d == 6);
}


TEST_CASE("Event should receive different Agent") {
    struct MyEvent : public Event<MyEvent> {
        int data = 0;
    };
    DEFINE_AGENT(SenderAgent) {
        void main() override {
            auto event = std::make_shared<MyEvent>();
            event->data = 42;
            emit<MyEvent>(event); // a = 1 d = 1
        }
    };

    DEFINE_AGENT(RXAgent) {
        int data = 0;

        void init() override {
            on<MyEvent>([this](auto event) {
                data = event->data;
            });
        }
    };
    auto agent = SenderAgent::spawnNewAgent();
    auto rx_agent = RXAgent::spawnNewAgent();
    while ( rx_agent->data != 42){}
    CHECK(rx_agent->data == 42);
}
