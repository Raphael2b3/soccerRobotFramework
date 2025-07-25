//
// Created by guita on 22.06.2025.
//

#include "messaging/priority/priority.h"
#include <stdexcept>
#include "messaging/event/event.h"
#define DEFAULT_PRIORITY 0

priority_t Priority::get_priority(const AgentId &sender_id) {
    if (priorityTable.contains(sender_id)) {
        return priorityTable[sender_id];
    }
    return DEFAULT_PRIORITY;
}
bool Priority::has_higher_priority(const BaseEvent *event, const BaseEvent *other_event) {
    if (!event || !other_event) {
        throw std::invalid_argument("Event pointers cannot be null");
    }

    priority_t event_sender_priority = get_priority(event->sender_id);
    priority_t other_event_sender_priority = get_priority(other_event->sender_id);

    bool event_priority_is_higher = event->priority > other_event->priority;
    bool event_priority_is_equal = event->priority == other_event->priority;
    bool sender_priority_is_higher = event_sender_priority > other_event_sender_priority;

    return event_priority_is_higher || (event_priority_is_equal && sender_priority_is_higher);
}

void Priority::set_sender_priority(AgentId id, priority_t priority)
{
    priorityTable.insert_or_assign(id,priority);
}