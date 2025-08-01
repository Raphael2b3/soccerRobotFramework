//
// Created by guita on 18.07.2025.
//

#ifndef AGENT_H
#define AGENT_H

#include <boost/thread.hpp>

#include "agent_messaging/agent_messaging.h"
#include "id/id.h"
#include "base_agent.h"
#include "pool/pool.h"

#define DEFINE_AGENT(name) \
struct name : public Agent<name>

#define DEFINE_AGENT_NAME(name) \
public: \
inline static std::string agent_name = name;

template <typename T>
class Agent : public AgentMessaging<T>
{

    boost::thread main_thread;
    boost::thread mailbox_handler_thread;
    inline static Pool<T> pool;

public:
    DEFINE_AGENT_NAME("UnkownAgent");
    DEFINE_POOL_SIZE(1);

    static std::shared_ptr<T> spawnNewAgent();
    static std::shared_ptr<T> getAgent(const AgentId& id)
    {
        return pool.getAgent(id);
    }
    static std::shared_ptr<T> getAgentLoadBalanced()
    {
        return pool.getAgentLoadBalanced();
    }

    void kill() final;
};


template <typename T>
std::shared_ptr<T> Agent<T>::spawnNewAgent()
{

    auto agent = pool.spawnNewAgentIfPoolAllows();
    if (!agent) return nullptr; // Check for null pointer
    agent->init();
    // Start main thread
    agent->main_thread = boost::thread([agent]()
    {
        assert(agent != nullptr && "Agent cannot be null (main thread)");
        agent->main();
    });
    // Start "other stuff" thread (loops forever)
    agent->mailbox_handler_thread = boost::thread([agent]()
    {
        while (agent->running)
        {
            boost::this_thread::interruption_point();
            if (!agent)
            {
                throw std::runtime_error("Agent cannot be null in mailbox handler thread");
                return; // Exit thread if agent is null or not running
            }
            auto event = agent->mailbox.getNextEvent(); // pop front doesnt exist
            if (event!= nullptr)
                agent->event_handler.dispatch(event);
        }
    });

    return agent; // caller responsible for managing memory
}

template <typename T>
void Agent<T>::kill()
{
    this->running = false;
    this->event_handler.clear_all_handlers();
    pool.deleteAgent(this->runtime_id);
    mailbox_handler_thread.interrupt();
    main_thread.interrupt();
    mailbox_handler_thread.join(); // <-- Wait for both to stop safely
    main_thread.join();
    this->unsubscribe_all();

}


#endif //AGENT_H
