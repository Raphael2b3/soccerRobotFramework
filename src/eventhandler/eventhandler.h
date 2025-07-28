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
    std::map<std::type_index, std::vector<std::function<void(std::shared_ptr<BaseEvent>)>>> handlers;

    // Register a callback for a specific event type T
    template <typename T>
    void register_callback(std::function<void(std::shared_ptr<T>)> callback) {
        // Wrap to a function taking BaseEvent&
        assert(callback != nullptr && "Callback cannot be null");

        auto wrapper = [callback](std::shared_ptr<BaseEvent> event) {
            // Dynamic cast to ensure runtime safety
            auto typed_event = std::dynamic_pointer_cast<T>(event);
            assert(typed_event && "Event type mismatch in callback");
            callback(typed_event);
        };

        auto type = std::type_index(typeid(T));
        assert(type != std::type_index(typeid(BaseEvent)) && "Cannot register BaseEvent directly, use derived types");
        handlers[type].push_back(wrapper);
    }

    void dispatch(const std::shared_ptr<BaseEvent>& event) {
        assert(event!= nullptr && "Event cannot be null");
        auto type = std::type_index(typeid(*event));
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
