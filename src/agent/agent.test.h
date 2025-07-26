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

DEFINE_AGENT(TestAgent)
{
    DEFINE_POOL_SIZE(50)
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
    DEFINE_AGENT(Agent1)
    {
    };
    DEFINE_AGENT(Agent2)
    {
    };
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

    auto fetched = TestAgent::getAgent(a->runtime_id);
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
DEFINE_AGENT(FreshAgent)
{
    DEFINE_POOL_SIZE(POOL_SIZE_LIMIT)
};

TEST_CASE("Agent pool respects limit")
{
    bool has_failed = false;
    std::vector<std::shared_ptr<FreshAgent>> agents;
    for (int i = 0; i < POOL_SIZE_LIMIT + 1; ++i)
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
    DEFINE_AGENT(AssertingTestAgent1)
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
    // _sleep(100); //it takes some time for tha main thread to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CHECK(agent->main_thread_started == true);
    agent->kill();
}


TEST_CASE("Agent should not call deconstructor on kill")
{
    auto disposed = std::make_shared<bool>(false);


    DEFINE_AGENT(AssertingTestAgent2)
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

    CHECK(*disposed == false);
}


TEST_CASE("Agent should not call deconstructor when getting out of scope (no kill function run)")
{
    int disposed = 0;
    {
        DEFINE_AGENT(AssertingTestAgent3)
        {
        public:
            int* disposed_ptr;

            ~AssertingTestAgent3() override
            {
                *disposed_ptr = 42; // Set the flag when dispose is called
            }
        };

        auto agent = AssertingTestAgent3::spawnNewAgent();
        agent->disposed_ptr = &disposed;
    }
    CHECK(disposed == 0);
}


TEST_CASE("Agent should call deconstructor when kill function was run and then getting out of scope")
{
    int disposed = 0;
    {
        DEFINE_AGENT(AssertingTestAgent3)
        {
        public:
            int* disposed_ptr;

            ~AssertingTestAgent3() override
            {
                *disposed_ptr = 42; // Set the flag when dispose is called
            }
        };

        auto agent = AssertingTestAgent3::spawnNewAgent();
        agent->disposed_ptr = &disposed;
        agent->kill();
    }
    CHECK(disposed == 42);
}

TEST_CASE("Agent should call deconstructor when getting out of scope and kill function was run from another scope")
{
    int disposed = 0;
    DEFINE_AGENT(AssertingTestAgent3)
    {
    public:
        int* disposed_ptr;

        ~AssertingTestAgent3() override
        {
            *disposed_ptr = 42; // Set the flag when dispose is called
        }
    };
    AgentId id;
    {
        auto agent = AssertingTestAgent3::spawnNewAgent();
        agent->disposed_ptr = &disposed;
        id = agent->runtime_id; // Store the ID to kill later
    }
    {
        auto agent = AssertingTestAgent3::getAgent(id);
        CHECK(agent != nullptr);
        agent->kill(); // Kill the agent from another scope
    }

    CHECK(disposed == 42);
}


TEST_CASE("Killing an Agent should remove it from pool")
{
    DEFINE_AGENT(AssertingTestAgent3)
    {
    };
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

    DEFINE_AGENT(AssertingTestAgent3)
    {
    };
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
