//
// Created by guita on 22.06.2025.
//

#include "messaging/priority/priority.h"
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

TEST_CASE("Higher event priority wins regardless of sender") {
    Priority prio;

    BaseEvent e1(AgentId::getNewId("test"), 5);
    BaseEvent e2(AgentId::getNewId("test"), 3);

    CHECK(prio.has_higher_priority(&e1, &e2)); // e1 has higher event priority
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1)); // e2 has lower priority
}

TEST_CASE("Equal event priority, higher sender priority wins") {
    Priority prio;
    auto id1 = AgentId::getNewId("test");
    auto id2 = AgentId::getNewId("test");
    BaseEvent e1(id1, 3);
    BaseEvent e2(id2, 3);

    prio.set_sender_priority(id1, 10);
    prio.set_sender_priority(id2, 5);

    CHECK(prio.has_higher_priority(&e1, &e2)); // senderA > senderB
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1));
}

TEST_CASE("Equal event and sender priority = no higher priority") {
    Priority prio;

    BaseEvent e1(AgentId::getNewId("test"), 3);
    BaseEvent e2(AgentId::getNewId("test"), 3); // same sender, same prio

    CHECK_FALSE(prio.has_higher_priority(&e1, &e2));
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1));
}

TEST_CASE("Default sender priority applies when not set") {
    Priority prio;

    BaseEvent e1(AgentId::getNewId("test"), 2);
    BaseEvent e2(AgentId::getNewId("test"), 2);

    // No sender priority set — should default to 0
    CHECK_FALSE(prio.has_higher_priority(&e1, &e2));
}

TEST_CASE("Null event throws exception") {
    Priority prio;
    BaseEvent dummy(AgentId::getNewId("test"), 1);

    CHECK_THROWS_AS(prio.has_higher_priority(nullptr, &dummy), std::invalid_argument);
    CHECK_THROWS_AS(prio.has_higher_priority(&dummy, nullptr), std::invalid_argument);
    CHECK_THROWS_AS(prio.has_higher_priority(nullptr, nullptr), std::invalid_argument);
}