//
// Created by guita on 20.06.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include <vector>
#include <doctest/doctest.h>

#include "types.h"
#include "agent/agent.h"
#include "baseevent.h"

template <typename T>
class Event : public BaseEvent
{
public:
    inline static std::map<AgentId, std::shared_ptr<IAgent>> subscribers;
    inline static bool initialized = false;

    static void initialize()
    {
        if (initialized)
            return;
        initialized = true;
    }

    static void subscribe(std::shared_ptr<IAgent> agent)
    {
        subscribers[agent->runtime_id] = agent;
    }

    static void unsubscribe(std::shared_ptr<IAgent> agent)
    {
        subscribers.erase(agent->runtime_id);
    }
    static void emit(std::shared_ptr<IAgent> agent, std::shared_ptr<T> e)
    {
        for (auto it = subscribers.begin(); it != subscribers.end(); ++it)
        {
            it->second->mailbox.mail(e);
        }

    }
};
#endif //EVENT_H
