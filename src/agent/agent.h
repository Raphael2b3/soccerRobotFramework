//
// Created by guita on 18.07.2025.
//

#ifndef AGENT_H
#define AGENT_H

#include <map>
#include <boost/thread.hpp>
#include "id/id.h"
#include "iagent.h"
#include "pool/pool.h"
#define DEFAULT_POOL_SIZE 1

#define SET_POOL_SIZE_LIMIT(number) public: inline static size_t max_pool_size = number; //TODO make this cleaner


template <typename T>
class Agent : public IAgent
{
public:

    inline static std::string agent_name = "UnknownAgent";

    inline static Pool<T> pool;
    inline static size_t max_pool_size = DEFAULT_POOL_SIZE;

    static std::shared_ptr<T> spawnNewAgent();

    void kill() final;
private:

    boost::thread main_thread;
    boost::thread mailbox_handler_thread;
};


template <typename T>
std::shared_ptr<T> Agent<T>::spawnNewAgent()
{

    auto agent = T::pool.spawnNewAgent();
    if (!agent) return nullptr;

    agent->init();
    // Start main thread
    agent->main_thread = boost::thread([agent]()
    {
        agent->main();
    });

    // Start "other stuff" thread (loops forever)
    agent->mailbox_handler_thread = boost::thread([]()
    {
        while (1)
        {
            boost::this_thread::interruption_point();
            // Todo fix that
            // todo needs testage
           // auto event = agent.mailbox.mailbox.pop_front(); // pop front doesnt exist
           // agent.event_handler.dispatch(event);

        }
    });

    return agent; // caller responsible for managing memory
}

template <typename T>
void Agent<T>::kill()
{
    mailbox_handler_thread.interrupt();
    main_thread.interrupt();
    mailbox_handler_thread.join();  // <-- Wait for both to stop safely
    main_thread.join();

    T::pool.deleteAgent(this->runtime_id);
}


#endif //AGENT_H
