//
// Created by guita on 18.07.2025.
//

#ifndef AGENT_H
#define AGENT_H

#include <map>
#include <boost/thread.hpp>
#include "id/id.h"
#include "iagent.h"
#define DEFAULT_POOL_SIZE 1

#define SET_POOL_SIZE_LIMIT(number) public: inline static size_t max_pool_size = number; //TODO make this cleaner


template <typename T>
class Agent : public IAgent
{
public:
    inline static std::string agent_name = "UnknownAgent";

    inline static size_t max_pool_size = DEFAULT_POOL_SIZE;

    static T* spawnNewAgent();

    static T* getAgent(AgentId id);

    static T* getAgentLoadBalanced();

    void kill() final;

private:
    inline static std::map<AgentId, IAgent*> agent_pool;
    inline static boost::mutex agent_mutex;

    boost::thread main_thread;
    boost::thread mailbox_handler_thread;
};


template <typename T>
T* Agent<T>::spawnNewAgent()
{
    if (T::agent_pool.size() >= T::max_pool_size)
    {
        //printf("Agent pool is full, cannot spawn new agent.\n");
        return nullptr; // Pool is full
    }
    auto agent = new T();
    agent->runtime_id = AgentId::getNewId(agent_name);
    agent->init();
    // Start main thread
    agent->main_thread = boost::thread(&T::main, agent);

    // Start "other stuff" thread (loops forever)
    agent->mailbox_handler_thread = boost::thread([]()
    {
        while (1)
        {
            boost::this_thread::sleep_for(boost::chrono::seconds(1));
            printf("Agent %s is doing other stuff...\n","");// agent->runtime_id.toString().c_str());
        }
    });
    boost::lock_guard<boost::mutex> lock(T::agent_mutex);

    agent_pool[agent->runtime_id] = agent;
    return agent; // caller responsible for managing memory
}

template <typename T>
T* Agent<T>::getAgent(AgentId id)
{
    boost::lock_guard<boost::mutex> lock(T::agent_mutex);
    auto it = T::agent_pool.find(id);
    if (it != T::agent_pool.end())
    {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template <typename T>
T* Agent<T>::getAgentLoadBalanced()
{
    //TODO Implement Round Robin
    //TODO needs testage
    return nullptr;
}

template <typename T>
void Agent<T>::kill()
{
    mailbox_handler_thread.interrupt();
    main_thread.interrupt();
    mailbox_handler_thread.join();  // <-- Wait for both to stop safely
    main_thread.join();

    printf("threads have been stopped");
    boost::lock_guard<boost::mutex> lock(agent_mutex);
    agent_pool.erase(runtime_id);
    delete this;
}


#endif //AGENT_H
