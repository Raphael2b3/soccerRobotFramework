//
// Created by guita on 20.06.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include "agent/agent.h"
#include "baseevent.h"

template <typename T>
class Event : public BaseEvent
{
    inline static std::map<AgentId, std::shared_ptr<IAgent>> subscribers;
    inline static std::mutex subscribers_mutex;

public:

    template <typename Backend>
    static void internal_emit(std::shared_ptr<T> event) //TODO find better name for this method
    {
        // Todo check if another backend has already send the event
        // TODO only send the event if the backend is the right one for the handler
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(event != nullptr && "Event cannot be null (emit)");
        for (auto it = subscribers.begin(); it != subscribers.end(); ++it)
        {
            it->second->mailbox.mail(event);
        }
    }

    template <typename... Backend>
    static void subscribe(const std::shared_ptr<IAgent>& agent)
    {
        (Backend::template init<T>(),...);
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(agent != nullptr && "Agent cannot be null (subscribe)");
        subscribers[agent->runtime_id] = agent;
    }

    static void unsubscribe(const std::shared_ptr<IAgent>& agent)
    {
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(agent != nullptr && "Agent cannot be null (subscribe)");
        subscribers.erase(agent->runtime_id);
    }

    template <typename... Backend>
    static void emit(std::shared_ptr<T> event)
    {
        (Backend::template emit<T>(event),...);
    }
};

#endif //EVENT_H
