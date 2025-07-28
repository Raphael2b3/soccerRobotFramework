//
// Created by guita on 20.06.2025.
//

#ifndef MAILBOX_H
#define MAILBOX_H
#include <vector>
#include "messaging/priority/priority.h"

#define MAX_MAILBOX_SIZE 1024

class Mailbox {
    Priority priorityManager;

    size_t getIndexToInsert(std::shared_ptr<BaseEvent> event);

public:
    std::vector<std::shared_ptr<BaseEvent> > mailbox; // Public for testability
    bool mail(std::shared_ptr<BaseEvent> e);
};
#endif //MAILBOX_H
