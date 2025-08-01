//
// Created by guita on 20.06.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include "agent/agent.h"
#include "base_event.h"
#include "subscribtion_key.h"

template<typename T>
class Event : public BaseEvent {
    inline static std::map<SubscriptionKey, std::shared_ptr<BaseAgent> > subscribers;
    inline static std::mutex subscribers_mutex;

public:
    template<typename Backend>
    static void dispatch_event(std::shared_ptr<T> event)
    {
        // Todo check if another backend has already send the event
        // TODO only send the event if the backend is the right one for the handler
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(event != nullptr && "Event cannot be null (emit)");
        for (auto it = subscribers.begin(); it != subscribers.end(); ++it) {
            if (it->first.backend_type == std::type_index(typeid(Backend))) {
                it->second->mailbox.mail(event);
            }
        }
    }

    template<typename... Backend>
    static void subscribe(const std::shared_ptr<BaseAgent> &agent) {
        (Backend::template init<T>(), ...);
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(agent != nullptr && "Agent cannot be null (subscribe)");
        ((subscribers[subscription_key<Backend>(agent->runtime_id)] = agent), ...);
    }

    template<typename... Backend>
    static void unsubscribe(const std::shared_ptr<BaseAgent> &agent) {
        std::lock_guard<std::mutex> lock(subscribers_mutex);
        assert(agent != nullptr && "Agent cannot be null (subscribe)");
        (subscribers.erase(subscription_key<Backend>(agent->runtime_id)), ...);
    }

    template<typename... Backend>
    static void emit(std::shared_ptr<T> event) {
        (Backend::template emit<T>(event), ...);
    }
};

#endif //EVENT_H
