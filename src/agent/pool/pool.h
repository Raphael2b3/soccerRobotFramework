//
// Created by guita on 25.07.2025.
//

#ifndef POOL_H
#define POOL_H
#include <map>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>


#define DEFAULT_POOL_SIZE 1
#define DEFINE_POOL_SIZE(size) \
public: \
inline static size_t max_pool_size = size;

template <typename T>
class Pool
{
public:
    std::shared_ptr<T> getAgent(const AgentId& id);

    std::shared_ptr<T> getAgentLoadBalanced();

    std::shared_ptr<T> spawnNewAgentIfPoolAllows();

    void putAgent(std::shared_ptr<T> agent);

    void deleteAgent(const AgentId& id);

private:
    std::map<AgentId, std::shared_ptr<T>> agent_pool;
    std::mutex pool_list_mutex;
    typename std::map<AgentId, std::shared_ptr<T>>::iterator round_robin_iterator = agent_pool.end();
};


template <typename T>
std::shared_ptr<T> Pool<T>::spawnNewAgentIfPoolAllows()
{
    //TODO Maby even spawn on different Machine?
    if (agent_pool.size() >= T::max_pool_size)
    {
        return nullptr; // Pool is full
    }
    auto agent = std::make_shared<T>();
    agent->runtime_id = AgentId::getNewId(T::agent_name);
    putAgent(agent);
    return agent; // caller responsible for managing memory
}

template <typename T>
void Pool<T>::putAgent(std::shared_ptr<T> agent)
{
    assert(agent!=nullptr && "Agent cannot be null  (putAgent)");
    std::lock_guard lock(pool_list_mutex);
    agent_pool[agent->runtime_id] = agent;
}

template <typename T>
void Pool<T>::deleteAgent(const AgentId& id)
{
    std::lock_guard lock(pool_list_mutex);
    agent_pool.erase(id);
}

template <typename T>
std::shared_ptr<T> Pool<T>::getAgent(const AgentId& id)
{
    std::lock_guard lock(pool_list_mutex);
    auto it = agent_pool.find(id);
    if (it != agent_pool.end())
    {
        assert(it->second != nullptr && "Agent in pool cannot be null (getAgent, in loop)");
        return it->second;
    }
    return nullptr;
}

template <typename T>
std::shared_ptr<T> Pool<T>::getAgentLoadBalanced()
{
    std::lock_guard lock(pool_list_mutex);

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
    assert(round_robin_iterator->second != nullptr && "Agent in pool cannot be null (getAgentLoadBalanced)");
    return round_robin_iterator->second;
}


#endif //POOL_H
