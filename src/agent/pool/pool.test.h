#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "pool.h"
#include "id/id.h"
#include "agent/iagent.h"

class PoolTestAgent1 : public Agent<PoolTestAgent1>
{
public:
    inline static std::string agent_name = "11PoolTestAgent";
    inline static size_t max_pool_size = 30;
};


TEST_CASE("Agent Pool actual Spawns Agents")
{
    Pool<PoolTestAgent1> pool;

    auto agent = pool.spawnNewAgent();
    CHECK(agent != nullptr);
    CHECK(agent->runtime_id.agent_name == "11PoolTestAgent");

    // Clean up
    agent->kill();
}

class PoolTestAgent3 : public Agent<PoolTestAgent3>
{
public:
    inline static std::string agent_name = "22PoolTestAgent";
    inline static size_t max_pool_size = 20;
};

TEST_CASE("Pool should not exceed max_pool_size")
{
    Pool<PoolTestAgent3> pool;

    for (int i = 0; i < PoolTestAgent3::max_pool_size; ++i)
    {
        auto agent = pool.spawnNewAgent();
        CHECK(agent != nullptr);
    }

    // One more should fail
    auto agent = pool.spawnNewAgent();
    CHECK(agent == nullptr);
}

TEST_CASE("Pool should store and retrieve agent by ID")
{
    Pool<PoolTestAgent1> pool;

    auto agent = pool.spawnNewAgent();
    REQUIRE(agent != nullptr);

    auto id = agent->runtime_id;
    auto fetched = pool.getAgent(id);
    CHECK(fetched == agent);
}

TEST_CASE("Pool insertion should work")
{
    Pool<PoolTestAgent1> pool;

    auto agent = pool.spawnNewAgent();
    agent->runtime_id = AgentId::getNewId("InsertedAgent");

    pool.putAgent(agent);
    CHECK(pool.getAgent(agent->runtime_id) == agent);
    agent->kill();
}


TEST_CASE("Pool Agent deletion should work")
{
    Pool<PoolTestAgent1> pool;

    auto agent = pool.spawnNewAgent();
    agent->runtime_id = AgentId::getNewId("InsertedAgent");

    pool.putAgent(agent);
    CHECK(pool.getAgent(agent->runtime_id) == agent);

    pool.deleteAgent(agent->runtime_id);
    CHECK(pool.getAgent(agent->runtime_id) == nullptr);

}


TEST_CASE("Round Robin Load balancer Should work")
{
    Pool<PoolTestAgent1> pool;

    auto a = pool.spawnNewAgent();
    auto b = pool.spawnNewAgent();

    auto agent1 = pool.getAgentLoadBalanced();
    auto agent2 = pool.getAgentLoadBalanced();
    CHECK(agent1 != nullptr);
    CHECK(agent2 != nullptr);
    CHECK(agent1 != agent2); // Should return different agents
    a->kill();
    auto agent3 = pool.getAgentLoadBalanced();
    CHECK(agent3 == b); // Should return the remaining agent
}

TEST_CASE("Round robin returns agents in order")
{
    class RoundRobinTestAgent : public Agent<RoundRobinTestAgent>
    {};
    RoundRobinTestAgent::max_pool_size = 5;

    Pool<RoundRobinTestAgent> pool;

    auto agent1 = pool.spawnNewAgent();
    auto agent2 = pool.spawnNewAgent();
    auto agent3 = pool.spawnNewAgent();

    CHECK(agent1 != nullptr);
    CHECK(agent2 != nullptr);
    CHECK(agent3 != nullptr);

    // Store returned IDs
    auto a = pool.getAgentLoadBalanced();
    auto b = pool.getAgentLoadBalanced();
    auto c = pool.getAgentLoadBalanced();
    auto d = pool.getAgentLoadBalanced(); // should wrap

    CHECK(a == agent1);
    CHECK(b == agent2);
    CHECK(c == agent3);
    CHECK(d == agent1);
}