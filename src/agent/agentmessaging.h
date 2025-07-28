//
// Created by guita on 29.07.2025.
//

#ifndef AGENTMESSAGING_H
#define AGENTMESSAGING_H
#include <memory>

#include "iagent.h"
#include "eventhandler/eventhandler.h"
#include "messaging/backend/in_memory/in_memory_backend.h"

template <typename T>
class AgentMessaging : public std::enable_shared_from_this<T>, public IAgent
{
public:
    EventHandler event_handler;

    template <typename EventT, typename... BackendT>
    void on(std::function<void(std::shared_ptr<EventT>)> callback)
    {
        static_assert(std::is_base_of<BaseEvent, EventT>::value, "EventT must derive from BaseEvent.");
        auto self = this->shared_from_this();
        if constexpr (sizeof...(BackendT) > 0)
        {
            (EventT::template subscribe<BackendT>(self), ...);
        }
        else
        {
            EventT::template subscribe<InMemoryBackend>(self);
        }

        event_handler.register_callback<EventT>(callback);
    }

    template <typename EventT, typename... BackendT>
    void emit(std::shared_ptr<EventT> event)
    {
        static_assert(std::is_base_of<BaseEvent, EventT>::value,
                      "EventT must derive from BaseEvent.");
        event->sender_id = this->runtime_id;
        if constexpr (sizeof...(BackendT) > 0)
        {
            (EventT::template emit<BackendT>(event), ...);
        }
        else
        {
            EventT::template emit<InMemoryBackend>(event);
        }
    }
};
#endif //AGENTMESSAGING_H
