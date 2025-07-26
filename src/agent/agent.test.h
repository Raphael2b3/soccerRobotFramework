//
// Created by guita on 18.07.2025.
//

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <thread>
#include <chrono>
#include <utility>
#include <vector>
#include "agent/agent.h"

class TestAgent : public Agent<TestAgent>
{
    SET_POOL_SIZE_LIMIT(30)
};


TEST_CASE("Agent spawning and ID assignment")
{
    auto a1 = TestAgent::spawnNewAgent();
    CHECK(a1 != nullptr);

    auto a2 = TestAgent::spawnNewAgent();
    CHECK(a2 != nullptr);

    CHECK(a1->runtime_id != a2->runtime_id);

    a1->kill();
    a2->kill();
}

TEST_CASE("Agent ID agent number is counted globally")
{
    class Agent1:public Agent<Agent1>{};
    class Agent2:public Agent<Agent2>{};
    auto a1 = Agent1::spawnNewAgent();
    CHECK(a1 != nullptr);

    auto a2 = Agent2::spawnNewAgent();
    CHECK(a2 != nullptr);

    CHECK(a1->runtime_id.agent_number != a2->runtime_id.agent_number);
    a1->kill();
    a2->kill();
}

TEST_CASE("Agent retrieval works")
{
    auto a = TestAgent::spawnNewAgent();
    REQUIRE(a != nullptr);

    auto fetched = TestAgent::pool.getAgent(a->runtime_id);
    CHECK(fetched == a);
    a->kill();
}

TEST_CASE("Agent kill interrupts threads")
{
    auto a = TestAgent::spawnNewAgent();
    REQUIRE(a != nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a->kill();
}

#define POOL_SIZE_LIMIT 10
class FreshAgent: public Agent<FreshAgent>
{
    SET_POOL_SIZE_LIMIT(POOL_SIZE_LIMIT)
};

TEST_CASE("Agent pool respects limit")
{
    bool has_failed = false;
    std::vector<std::shared_ptr<FreshAgent>> agents;
    for (int i = 0; i < POOL_SIZE_LIMIT+1; ++i)
    {
        auto a = FreshAgent::spawnNewAgent();
        if (!a)
        {
            has_failed = true;
            CHECK(i >= POOL_SIZE_LIMIT);
            break;
        }
        agents.push_back(a);
    }
    CHECK(has_failed);

    for (auto a : agents)
    {
        a->kill();
    }
}

TEST_CASE("Agent should call init before main")
{
    class AssertingTestAgent1 : public Agent<AssertingTestAgent1>
    {
    public:
        bool initialized = false;
        bool main_thread_started = false;

        void init() override
        {
            initialized = true;
        }

        void main() override
        {
            CHECK(initialized == true);
            main_thread_started = true;
        }
    };

    auto agent = AssertingTestAgent1::spawnNewAgent();
    _sleep(100); //it takes some time for tha main thread to start
    CHECK(agent->main_thread_started == true);
    agent->kill();
}


TEST_CASE("Agent should call deconstructor on kill")
{
    auto disposed = std::make_shared<bool>(false);

    class AssertingTestAgent2 : public Agent<AssertingTestAgent2>
    {
    public:
        std::shared_ptr<bool> disposed_flag;

        ~AssertingTestAgent2() override
        {
            *disposed_flag = true; // Set the flag when dispose is called
        }
    };


    auto agent = AssertingTestAgent2::spawnNewAgent();
    agent->disposed_flag = disposed;
    agent->kill();

    CHECK(*disposed == true);
}

TEST_CASE("Agent should call deconstructor when getting out of scope")
{
    bool disposed = false;
    {
    class AssertingTestAgent3 : public Agent<AssertingTestAgent3>
    {
    public:
        bool* disposed_flag;


        ~AssertingTestAgent3() override
        {
            *disposed_flag = true; // Set the flag when dispose is called
        }
    };

    auto agent = AssertingTestAgent3::spawnNewAgent();
    agent->disposed_flag = &disposed;
    }
    CHECK(disposed == true);
}

TEST_CASE("Killing an Agent should remove it from pool")
{
    class AssertingTestAgent3 : public Agent<AssertingTestAgent3>{};
    //AssertingTestAgent3::max_pool_size = 1;

    auto agent = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent != nullptr);
    auto agent2 = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent2 == nullptr);
    agent->kill();
    auto agent3 = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent3 != nullptr);
    agent3->kill();
}

TEST_CASE("Changing max Pool size during runtime has an effect")
{
    class AssertingTestAgent3 : public Agent<AssertingTestAgent3>{};
    AssertingTestAgent3::max_pool_size = 1;

    auto agent = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent != nullptr);
    auto agent2 = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent2 == nullptr);
    AssertingTestAgent3::max_pool_size = 2;
    auto agent3 = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent3 != nullptr);
    auto agent4 = AssertingTestAgent3::spawnNewAgent();
    CHECK(agent4 == nullptr);
    agent->kill();
    agent3->kill();

}