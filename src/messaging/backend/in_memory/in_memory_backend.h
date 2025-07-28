//
// Created by guita on 20.06.2025.
//

#ifndef IN_MEMORY_BACKEND_H
#define IN_MEMORY_BACKEND_H
#include <vector>
#include <doctest/doctest.h>

#include "types.h"
#include "agent/agent.h"
#include "baseevent.h"

class InMemoryBackend
{
    template <typename T>
    static void emit(std::shared_ptr<T> event)
    {
        T::internal_emit(event);
    }
};
#endif //IN_MEMORY_BACKEND_H
