//
// Created by guita on 29.07.2025.
//

#ifndef AGENTMESSAGING_H
#define AGENTMESSAGING_H
#include <memory>

#include "agent/base_agent.h"
#include "eventhandler/eventhandler.h"
#include "messaging/backend/in_memory/in_memory_backend.h"
#define DEFAULT_BACKEND InMemoryBackend
template <typename T>
class AgentMessaging : public std::enable_shared_from_this<T>, public BaseAgent
{
    std::vector<std::function<void()>> unsubscribers;
public:
    EventHandler event_handler;

    template <typename EventT, typename... BackendT>
    requires std::derived_from<EventT, BaseEvent>
    void on(std::function<void(std::shared_ptr<EventT>)> callback)
    {
        auto self = this->shared_from_this();
        if constexpr (sizeof...(BackendT) > 0)
        {
            (EventT::template subscribe<BackendT>(self), ...);
        }
        else
        {
            EventT::template subscribe<DEFAULT_BACKEND>(self);
        }

        event_handler.register_callback<EventT>(callback);
        unsubscribers.push_back([self]() {
            EventT::unsubscribe(self);
        });
    }

    template <typename EventT, typename... BackendT>
    requires std::derived_from<EventT, BaseEvent> // TODO Fix so that EvenT must be BaseEvent Derivative and throws compiler error if not
    void emit(std::shared_ptr<EventT> event)
    {
        event->sender_id = this->runtime_id;
        if constexpr (sizeof...(BackendT) > 0)
        {
            (EventT::template emit<BackendT>(event), ...);
        }
        else
        {
            EventT::template emit<DEFAULT_BACKEND>(event);
        }
    }

    void unsubscribe_all() const
    {
        for (auto& unsub :unsubscribers)
        {
            unsub();
        }
    }
};
#endif //AGENTMESSAGING_H
