//
// Created by guita on 25.07.2025.
//

#ifndef BASEEVENT_H
#define BASEEVENT_H
#include "types.h"
#include "id/id.h"


class BaseEvent
{
public:
    AgentId sender_id;
    priority_t priority = 0;
    int data_size = 0;
    void* data = nullptr;

    bool operator==(const BaseEvent& event) const
    {
        return sender_id == event.sender_id && priority == event.priority && data_size == event.data_size;
    }
};


#endif //BASEEVENT_H
