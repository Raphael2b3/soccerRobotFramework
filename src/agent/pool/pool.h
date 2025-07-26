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
    std::shared_ptr<T> spawnNewAgent();

    std::shared_ptr<T> getAgent(AgentId id);

    std::shared_ptr<T> getAgentLoadBalanced();

    void putAgent(std::shared_ptr<T> agent)
    {
        if (!agent)
            return; // Check for null pointer
        boost::lock_guard<boost::mutex> lock(pool_list_mutex);
        agent_pool[agent->runtime_id] = agent;
    }

    void deleteAgent(const AgentId& id)
    {
        boost::lock_guard<boost::mutex> lock(pool_list_mutex);
        agent_pool.erase(id);
    }

private:
    std::map<AgentId, std::shared_ptr<T>> agent_pool;
    boost::mutex pool_list_mutex;
    typename std::map<AgentId, std::shared_ptr<T>>::iterator round_robin_iterator = agent_pool.end();
};


template <typename T>
std::shared_ptr<T> Pool<T>::spawnNewAgent()
{
    //TODO Maby even spawn on different Machine?
    if (agent_pool.size() >= T::max_pool_size)
    {
        //printf("Agent pool is full, cannot spawn new agent.\n");
        return nullptr; // Pool is full
    }
    auto agent = std::make_shared<T>();
    auto newId = AgentId::getNewId(T::agent_name); // TODO check return value
    agent->runtime_id = newId;

    agent_pool[agent->runtime_id] = agent;
    return agent; // caller responsible for managing memory
}

template <typename T>
std::shared_ptr<T> Pool<T>::getAgent(AgentId id)
{
    boost::lock_guard<boost::mutex> lock(pool_list_mutex);
    auto it = agent_pool.find(id);
    if (it != agent_pool.end())
    {
        return static_cast<std::shared_ptr<T>>(it->second);
    }
    return nullptr;
}

template <typename T>
std::shared_ptr<T> Pool<T>::getAgentLoadBalanced()
{
    boost::lock_guard<boost::mutex> lock(pool_list_mutex);

    if (agent_pool.empty())
        return nullptr;

    // Initialize iterator if not set or invalid
    if (round_robin_iterator == agent_pool.end())
        round_robin_iterator = agent_pool.begin();
    else
        ++round_robin_iterator;

    // Wrap around
    if (round_robin_iterator == agent_pool.end())
        round_robin_iterator = agent_pool.begin();

    return static_cast<std::shared_ptr<T>>(round_robin_iterator->second);
}


#endif //POOL_H
