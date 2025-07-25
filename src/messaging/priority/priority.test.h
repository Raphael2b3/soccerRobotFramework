//
// Created by guita on 22.06.2025.
//

#include "./priority.h"
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

TEST_CASE("Higher event priority wins regardless of sender") {
    Priority prio;

    Event e1(0, 5);
    Event e2(1, 3);

    CHECK(prio.has_higher_priority(&e1, &e2)); // e1 has higher event priority
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1)); // e2 has lower priority
}

TEST_CASE("Equal event priority, higher sender priority wins") {
    Priority prio;

    Event e1(0, 3);
    Event e2(1, 3);

    prio.set_sender_priority(0, 10);
    prio.set_sender_priority(1, 5);

    CHECK(prio.has_higher_priority(&e1, &e2)); // senderA > senderB
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1));
}

TEST_CASE("Equal event and sender priority = no higher priority") {
    Priority prio;

    Event e1(0, 3);
    Event e2(1, 3); // same sender, same prio

    CHECK_FALSE(prio.has_higher_priority(&e1, &e2));
    CHECK_FALSE(prio.has_higher_priority(&e2, &e1));
}

TEST_CASE("Default sender priority applies when not set") {
    Priority prio;

    Event e1(0, 2);
    Event e2(1, 2);

    // No sender priority set — should default to 0
    CHECK_FALSE(prio.has_higher_priority(&e1, &e2));
}

TEST_CASE("Null event throws exception") {
    Priority prio;
    Event dummy(0, 1);

    CHECK_THROWS_AS(prio.has_higher_priority(nullptr, &dummy), std::invalid_argument);
    CHECK_THROWS_AS(prio.has_higher_priority(&dummy, nullptr), std::invalid_argument);
    CHECK_THROWS_AS(prio.has_higher_priority(nullptr, nullptr), std::invalid_argument);
}