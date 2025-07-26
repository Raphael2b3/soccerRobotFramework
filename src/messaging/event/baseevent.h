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

    bool operator==(const BaseEvent * other) const
    {
        return  other != nullptr && other == this;
    }
};


#endif //BASEEVENT_H
