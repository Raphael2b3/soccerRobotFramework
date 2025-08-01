//
// Created by guita on 20.06.2025.
//

#ifndef IN_MEMORY_BACKEND_H
#define IN_MEMORY_BACKEND_H
#include "agent/agent.h"

class InMemoryBackend {
public:

    template<typename T>
    static void init() {
    }

    template<typename T>
    static void emit(std::shared_ptr<T> event) {

        T::template dispatch_event<InMemoryBackend>(event);
    }
};
#endif //IN_MEMORY_BACKEND_H
