//
// Created by guita on 24.07.2025.
//
#include <functional>
#include <typeindex>

#include "../event/event.h"
class EventListener {
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

