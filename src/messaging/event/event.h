//
// Created by guita on 20.06.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include "../../types.h"

class Event
{

public:
    static void subscribe()
    {

    }
    static void unsubscribe()
    {

    }
    static void emit(Event e)
    {

    }
    sender_id_t sender_id = 0;
    priority_t priority = 0;
    int data_size = 0;
    void* data = nullptr;

    bool operator==(const Event& other) const
    {
        return sender_id == other.sender_id
        && priority == other.priority
        && data_size == other.data_size
        && data == other.data;
    }
};

#endif //EVENT_H
