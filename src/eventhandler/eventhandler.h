//
// Created by guita on 26.07.2025.
//

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include <functional>
#include <map>
#include <typeindex>
#include <vector>

#include "messaging/event/baseevent.h"


class EventHandler { // Instance is per agent instance and will be private.
public:
    // Map from event type to handlers
    std::map<std::type_index, std::vector<std::function<void(const BaseEvent&)>>> handlers;

    // Register a callback for a specific event type T
    template <typename T>
    void register_callback(std::function<void(const T&)> callback) {
        // Wrap to a function taking BaseEvent&
        assert(callback != nullptr && "Callback cannot be null");

        auto wrapper = [callback](const BaseEvent& event) {
            // Safe downcast (you must ensure correct type!)
            callback(static_cast<const T&>(event));
        };

        auto type = std::type_index(typeid(T));
        assert(type != std::type_index(typeid(BaseEvent)) && "Cannot register BaseEvent directly, use derived types");
        handlers[type].push_back(wrapper);
    }
    template <typename T>
    // Dispatch event to all relevant handlers
    void dispatch(const T& event) {
        assert(event!= nullptr && "Event cannot be null");
        auto type = std::type_index(typeid(event));
        assert(type != std::type_index(typeid(BaseEvent)) && "Cannot dispatch BaseEvent directly, use derived types");
        auto it = handlers.find(type);
        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                assert(handler != nullptr && "Handler cannot be null");
                handler(event);  // Call the wrapped handler
            }
        }
    }
};


#endif //EVENTHANDLER_H
