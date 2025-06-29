//
// Created by guita on 20.06.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include "../types.h"

struct event_t {
    sender_id_t sender_id = 0;
    priority_t priority = 0;
    int data_size = 0;
    void *data = nullptr;
};

#endif //EVENT_H
