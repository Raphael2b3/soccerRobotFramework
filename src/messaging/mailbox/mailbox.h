//
// Created by guita on 20.06.2025.
//

#ifndef MAILBOX_H
#define MAILBOX_H
#include "../event/event.h"
#include "../priority/priority.h"
#include <vector>

#define MAX_MAILBOX_SIZE 1024

class Mailbox {
    Priority priorityManager;
    size_t getIndexToInsert(const Event* event);
public:
    std::vector<Event *> mailbox; // Public for testability
    bool mail(Event *e);
};
#endif //MAILBOX_H
