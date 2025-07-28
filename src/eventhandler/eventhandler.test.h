//
// Created by guita on 18.07.2025.
//

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "agent/agent.h"


// Dummy event type for testing
class TestEvent : public BaseEvent
{
public:
    int value = 0;
};

// Another dummy event
class OtherEvent : public BaseEvent
{
public:
    std::string message;
};

TEST_CASE("EventHandler registers and dispatches events correctly")
{
    EventHandler handler;

    bool test_called = false;
    int test_value = 0;

    handler.register_callback<TestEvent>([&](std::shared_ptr<TestEvent> e)
    {
        test_called = true;
        test_value = e->value;
    });

    auto event = std::make_shared<TestEvent>();
    event->value = 42;

    handler.dispatch(event);

    CHECK(test_called);
    CHECK(test_value == 42);
}

TEST_CASE("EventHandler does not call handler for unrelated event types")
{
    EventHandler handler;

    bool test_called = false;

    handler.register_callback<TestEvent>([&](std::shared_ptr<TestEvent> e)
    {
        test_called = true;
    });

    auto event = std::make_shared<OtherEvent>();
    event->message = "Hello";

    handler.dispatch(event);

    CHECK_FALSE(test_called);
}

TEST_CASE("EventHandler supports multiple handlers for the same event type")
{
    EventHandler handler;

    int counter = 0;

    handler.register_callback<TestEvent>([&](std::shared_ptr<TestEvent>)
    {
        counter++;
    });

    handler.register_callback<TestEvent>([&](std::shared_ptr<TestEvent>)
    {
        counter++;
    });

    auto event = std::make_shared<TestEvent>();
    handler.dispatch(event);

    CHECK(counter == 2);
}
