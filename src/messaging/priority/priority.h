//
// Created by guita on 22.06.2025.
//

#ifndef PRIORITY_H
#define PRIORITY_H
#include <map>
#include "../event/event.h"
#include "../../types.h"


class Priority {
    std::map<sender_id_t, priority_t> priorityTable;

    priority_t get_priority(const sender_id_t &sender_id);

public:
    bool has_higher_priority(const Event *event, const Event *other_event);
    void set_sender_priority(sender_id_t id, priority_t priority);
};
#endif //PRIORITY_H
