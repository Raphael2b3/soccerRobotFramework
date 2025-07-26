#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "pool.h"
#include "id/id.h"
#include "agent/iagent.h"


DEFINE_AGENT(PoolTest1Agent)
{
    DEFINE_POOL_SIZE(30)
    DEFINE_AGENT_NAME("PoolTest1Agent")
};


TEST_CASE("Agent Pool actual Spawns Agents")
{
    auto agent = PoolTest1Agent::spawnNewAgent();
    CHECK(agent != nullptr);
    CHECK(agent->runtime_id.agent_name == "PoolTest1Agent");

    // Clean up
    agent->kill();
}

DEFINE_AGENT(PoolTestAgent3)
{
    DEFINE_POOL_SIZE(20)
    DEFINE_AGENT_NAME("PoolTestAgent3")
};

TEST_CASE("Pool should not exceed max_pool_size")
{
    for (int i = 0; i < PoolTestAgent3::max_pool_size; ++i)
    {
        auto agent = PoolTestAgent3::spawnNewAgent();
        CHECK(agent != nullptr);
    }

    // One more should fail
    auto agent = PoolTestAgent3::spawnNewAgent();
    CHECK(agent == nullptr);
}

TEST_CASE("Pool should store and retrieve agent by ID")
{
    auto agent = PoolTest1Agent::spawnNewAgent();
    REQUIRE(agent != nullptr);

    auto id = agent->runtime_id;
    auto fetched = PoolTest1Agent::getAgent(id);
    CHECK(fetched == agent);
}


TEST_CASE("Pool insertion should work")
{
    DEFINE_AGENT(PoolTestAgent2){};
    Pool<PoolTestAgent2> pool;

    auto agent = pool.spawnNewAgentIfPoolAllows();
    agent->runtime_id = AgentId::getNewId("InsertedAgent");
    pool.putAgent(agent);
    CHECK(pool.getAgent(agent->runtime_id) == agent);


}


TEST_CASE("Pool Agent deletion should work")
{
    DEFINE_AGENT(PoolTestAgent2){};
    Pool<PoolTestAgent2> pool;
    auto agent = pool.spawnNewAgentIfPoolAllows();
    agent->runtime_id = AgentId::getNewId("InsertedAgent");

    pool.putAgent(agent);
    CHECK(pool.getAgent(agent->runtime_id) == agent);

    pool.deleteAgent(agent->runtime_id);
    CHECK(pool.getAgent(agent->runtime_id) == nullptr);
}


TEST_CASE("Round Robin Load balancer Should work")
{
    DEFINE_AGENT(RoundRobinTestAgent)
    {
    };
    RoundRobinTestAgent::max_pool_size = 5;

    auto a = RoundRobinTestAgent::spawnNewAgent();
    auto b = RoundRobinTestAgent::spawnNewAgent();

    auto agent1 = RoundRobinTestAgent::getAgentLoadBalanced();
    auto agent2 = RoundRobinTestAgent::getAgentLoadBalanced();
    CHECK(agent1 != nullptr);
    CHECK(agent2 != nullptr);
    CHECK(agent1 != agent2); // Should return different agents
    a->kill();
    auto agent3 = RoundRobinTestAgent::getAgentLoadBalanced();
    CHECK(agent3 == b); // Should return the remaining agent
}

TEST_CASE("Round robin returns agents in order")
{
    DEFINE_AGENT(RoundRobinTestAgent)
    {
    };
    RoundRobinTestAgent::max_pool_size = 5;

    auto agent1 = RoundRobinTestAgent::spawnNewAgent();
    auto agent2 = RoundRobinTestAgent::spawnNewAgent();
    auto agent3 = RoundRobinTestAgent::spawnNewAgent();

    CHECK(agent1 != nullptr);
    CHECK(agent2 != nullptr);
    CHECK(agent3 != nullptr);

    // Store returned IDs
    auto a = RoundRobinTestAgent::getAgentLoadBalanced();
    auto b = RoundRobinTestAgent::getAgentLoadBalanced();
    auto c = RoundRobinTestAgent::getAgentLoadBalanced();
    auto d = RoundRobinTestAgent::getAgentLoadBalanced(); // should wrap

    CHECK(a == agent1);
    CHECK(b == agent2);
    CHECK(c == agent3);
    CHECK(d == agent1);
}
