//
// Created by guita on 25.07.2025.
//

#ifndef BASEEVENT_H
#define BASEEVENT_H
#include "types.h"
#include "id/id.h"


struct BaseEvent // TODO Maby even allow to send less data to trade off priority for data size
{ // TODO create a nice Event constructor so it is intuitive and easy to use.
    AgentId sender_id;
    priority_t priority = 0;

    BaseEvent(AgentId sender, priority_t p)
        : sender_id(std::move(sender)), priority(p) {}
    BaseEvent() = default;

    virtual ~BaseEvent() = default;
    bool operator==(const BaseEvent * other) const
    {
        return  other != nullptr && other == this;
    }
};


#endif //BASEEVENT_H
