//
// Created by guita on 18.07.2025.
//

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <chrono>
#include "agent/agent.h"


// Dummy event type for testing
struct TestEvent : public BaseEvent {
    int value = 0;
};

// Another dummy event
struct OtherEvent : public BaseEvent {
    std::string message;
};

TEST_CASE("EventHandler registers and dispatches events correctly") {
    EventHandler handler;

    bool test_called = false;
    int test_value = 0;

    handler.register_callback<TestEvent>([&](const TestEvent& e) {
        test_called = true;
        test_value = e.value;
    });

    TestEvent event;
    event.value = 42;

    handler.dispatch(event);

    CHECK(test_called);
    CHECK(test_value == 42);
}

TEST_CASE("EventHandler does not call handler for unrelated event types") {
    EventHandler handler;

    bool test_called = false;

    handler.register_callback<TestEvent>([&](const TestEvent& e) {
        test_called = true;
    });

    OtherEvent event;
    event.message = "Hello";

    handler.dispatch(event);

    CHECK_FALSE(test_called);
}

TEST_CASE("EventHandler supports multiple handlers for the same event type") {
    EventHandler handler;

    int counter = 0;

    handler.register_callback<TestEvent>([&](const TestEvent&) {
        counter++;
    });

    handler.register_callback<TestEvent>([&](const TestEvent&) {
        counter++;
    });

    TestEvent event;
    handler.dispatch(event);

    CHECK(counter == 2);
}