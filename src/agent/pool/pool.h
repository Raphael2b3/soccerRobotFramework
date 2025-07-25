//
// Created by guita on 25.07.2025.
//

#ifndef POOL_H
#define POOL_H
#include <map>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include "agent/iagent.h"

template <typename T>
class Pool
{
public:
    T* spawnNewAgent();

    T* getAgent(agent_id_t id);

    T* getAgentLoadBalanced();
    void deleteAgent(agent_id_t id);


private:
    std::map<agent_id_t, IAgent*> agent_pool;
    agent_id_t id_counter = 0;
    boost::mutex agent_mutex;
};


template <typename T>
T* Pool<T>::spawnNewAgent()
{
    //TODO Maby even spawn on different Machine?
    if (agent_pool.size() >= T::max_pool_size)
    {
        //printf("Agent pool is full, cannot spawn new agent.\n");
        return nullptr; // Pool is full
    }
    auto agent = new T();
    internal_init(agent);
    return agent; // caller responsible for managing memory
}

template <typename T>
T* Pool<T>::getAgent(int id)
{
    boost::lock_guard<boost::mutex> lock(agent_mutex);
    auto it = agent_pool.find(id);
    if (it != agent_pool.end())
    {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template <typename T>
T* Pool<T>::getAgentLoadBalanced()
{
    //TODO Implement Round Robin
    //TODO needs testage
    return nullptr;
}

template <typename T>
void Pool<T>::kill()
{
    mailbox_handler_thread.interrupt();
    main_thread.interrupt();
    boost::lock_guard<boost::mutex> lock(agent_mutex);
    agent_pool.erase(runtime_id);
    delete this;
}

template <typename T>
void Pool<T>::internal_init(T* agent)
{
    agent->runtime_id = T::id_counter++;
    agent->init();
    // Start main thread
    agent->main_thread = boost::thread(&T::main, agent);

    // Start "other stuff" thread (loops forever)
    agent->mailbox_handler_thread = boost::thread([agent]()
    {
        while (true)
        {
            boost::this_thread::sleep_for(boost::chrono::seconds(1));
            printf("Agent %d is doing other stuff...\n", agent->runtime_id);
        }
    });
    boost::lock_guard<boost::mutex> lock(T::agent_mutex);

    agent_pool[agent->runtime_id] = agent;
}
#endif //POOL_H
