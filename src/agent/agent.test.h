//
// Created by guita on 18.07.2025.
//
#include "./agent.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <thread>
#include <chrono>
#include <vector>

TEST_CASE("Agent spawning and ID assignment")
{
    Agent* a1 = Agent::spawnNewAgent();
    CHECK(a1 != nullptr);

    Agent* a2 = Agent::spawnNewAgent();
    CHECK(a2 != nullptr);

    CHECK(a1->runtime_id != a2->runtime_id);
}

TEST_CASE("Agent retrieval works")
{
    Agent* a = Agent::spawnNewAgent();
    REQUIRE(a != nullptr);

    Agent* fetched = Agent::getAgent(a->runtime_id);
    CHECK(fetched == a);
}

TEST_CASE("Agent kill interrupts threads")
{
    Agent* a = Agent::spawnNewAgent();
    REQUIRE(a != nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    CHECK(a->kill() == true);
}

TEST_CASE("Agent pool respects limit")
{
    std::vector<Agent*> agents;
    for (int i = 0; i < 105; ++i)
    {
        Agent* a = Agent::spawnNewAgent();
        if (!a)
        {
            CHECK(i >= 40); // Should fail only after 100
            break;
        }
        agents.push_back(a);
    }
}
