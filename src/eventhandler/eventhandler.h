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


class EventHandler {
public:
    // Map from event type to handlers
    std::map<std::type_index, std::vector<std::function<void(const BaseEvent&)>>> handlers;

    // Register a callback for a specific event type T
    template <typename T>
    void register_callback(std::function<void(const T&)> callback) {
        // Wrap to a function taking BaseEvent&
        auto wrapper = [callback](const BaseEvent& event) {
            // Safe downcast (you must ensure correct type!)
            callback(static_cast<const T&>(event));
        };

        handlers[typeid(T)].push_back(wrapper);
    }

    // Dispatch event to all relevant handlers
    void dispatch(const BaseEvent& event) {
        auto it = handlers.find(typeid(event));
        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);  // Call the wrapped handler
            }
        }
    }


};


#endif //EVENTHANDLER_H
