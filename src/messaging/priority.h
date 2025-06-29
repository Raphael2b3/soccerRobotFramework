//
// Created by guita on 22.06.2025.
//

#ifndef PRIORITY_H
#define PRIORITY_H
#include <map>
#include "event.h"
#include "../types.h"

#define DEFAULT_PRIORITY 0

class Priority {
    std::map<sender_id_t, priority_t> priorityTable;

    priority_t get_priority(const sender_id_t &sender_id);

public:
    bool has_higher_priority(const event_t *event, const event_t *other_event);
};
#endif //PRIORITY_H
