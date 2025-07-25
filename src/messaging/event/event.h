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
    inline static std::map<AgentId, IAgent*> subscribers;
    inline static bool initialized = false;

    static void initialize()
    {
        if (initialized)
            return;
        initialized = true;
    }

    static void subscribe(IAgent* agent)
    {
        subscribers[agent->runtime_id] = agent;
    }

    static void unsubscribe(IAgent* agent)
    {
        subscribers.erase(agent->runtime_id);
    }
    static void emit(IAgent* agent, T* e)
    {
        for (auto it = subscribers.begin(); it != subscribers.end(); ++it)
        {
            it->second->mailbox.mail(e);
        }

    }
};

/*class EventListener {

public:
    template <typename T>
    void on(std::function<void(const T&)> callback) {
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");

        // Lambda wrapper für Polymorphismus
        std::function<void(const Event&)> wrapper = [callback](const Event& e) {
            callback(static_cast<const T&>(e));
        };

        handlers[typeid(T)].push_back(wrapper);
    }

    void emit(const Event& event) {
        auto it = handlers.find(typeid(event));
        if (it != handlers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const Event&)>>> handlers;
};


*/
#endif //EVENT_H
