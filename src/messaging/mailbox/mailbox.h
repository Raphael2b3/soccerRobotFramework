//
// Created by guita on 20.06.2025.
//

#ifndef MAILBOX_H
#define MAILBOX_H
#include "../event.h"
#include "../priority/priority.h"
#include <vector>

#define MAX_MAILBOX_SIZE 1024

class Mailbox {
    Priority priorityManager;
    size_t getIndexToInsert(const event_t* event);
public:
    std::vector<event_t *> mailbox; // Public for testability
    bool mail(event_t *e);
};
#endif //MAILBOX_H
