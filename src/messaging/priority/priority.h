//
// Created by guita on 22.06.2025.
//

#ifndef PRIORITY_H
#define PRIORITY_H
#include <map>
#include "types.h"
#include "id/id.h"
#include "messaging/event/base_event.h"

class Priority {
    std::map<AgentId, priority_t> priorityTable;

    priority_t get_priority(const AgentId &sender_id);

public:
    bool has_higher_priority(std::shared_ptr<BaseEvent> event, std::shared_ptr<BaseEvent> other_event);
    void set_sender_priority(AgentId id, priority_t priority);
};
#endif //PRIORITY_H
