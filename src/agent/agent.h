//
// Created by guita on 18.07.2025.
//

#ifndef AGENT_H
#define AGENT_H


#include "../messaging/mailbox/mailbox.h"
#include <map>
#include <boost/thread.hpp>

#define agent_id_t int
#define DEFAULT_POOL_SIZE 1
#define SET_POOL_SIZE_LIMIT(number) public: inline static size_t max_pool_size = number;

template <typename T>
class Agent
{
public:
    inline static size_t max_pool_size = DEFAULT_POOL_SIZE;
    Mailbox mailbox;
    agent_id_t runtime_id;

    virtual void init()
    {
    };

    virtual void main()
    {
    };

    virtual ~Agent() = default;

    /**
     *  Delete this object. Dont use it after killing it.
     * @return
     */
    void kill()
    {
        mailbox_handler_thread.interrupt();
        main_thread.interrupt();
        boost::lock_guard<boost::mutex> lock(agent_mutex);
        agent_pool.erase(runtime_id);
        delete this;
    }

    static T* spawnNewAgent()
    {
        if (agent_pool.size() >= T::max_pool_size)
        {
            //printf("Agent pool is full, cannot spawn new agent.\n");
            return nullptr; // Pool is full
        }
        auto agent = new T();
        internal_init(agent);
        return agent; // caller responsible for managing memory
    }

    static T* getAgent(agent_id_t id)
    {
        boost::lock_guard<boost::mutex> lock(agent_mutex);
        auto it = agent_pool.find(id);
        if (it != agent_pool.end())
        {
            return it->second;
        }
        return nullptr;
    }

    static T* getAgentLoadBalanced()
    {
        //TODO implement Load balancing
    }

private:
    static void internal_init(T* agent)
    {
        agent->runtime_id = id_counter++;
        agent->init();
        boost::lock_guard<boost::mutex> lock(agent_mutex);
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

        agent_pool[agent->runtime_id] = agent;
    }

    inline static std::map<agent_id_t, T*> agent_pool;
    inline static agent_id_t id_counter = 0;
    inline static boost::mutex agent_mutex;

    boost::thread main_thread;
    boost::thread mailbox_handler_thread;
};

#endif //AGENT_H
