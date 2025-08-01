//
// Created by guita on 01.08.2025.
//

#ifndef SUBSCRIBTION_KEY_H
#define SUBSCRIBTION_KEY_H
struct SubscriptionKey {
    AgentId id;
    std::type_index backend_type;

    bool operator<(const SubscriptionKey& other) const {
        return std::tie(id, backend_type) < std::tie(other.id, other.backend_type);
    }
};

template <typename T>
SubscriptionKey subscription_key(AgentId id) {
    return SubscriptionKey{id, std::type_index(typeid(T))};
}
#endif //SUBSCRIBTION_KEY_H
