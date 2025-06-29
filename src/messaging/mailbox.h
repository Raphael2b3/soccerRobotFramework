//
// Created by guita on 20.06.2025.
//

#ifndef MAILBOX_H
#define MAILBOX_H
#include "./event.h"
#include <vector>

#include "priority.h"
#define MAX_MAILBOX_SIZE 1024

class Mailbox {
    std::vector<event_t *> mailbox;
    Priority priorityManager;
    size_t getIndexToInsert();
public:
    bool mail(event_t *e);
};
#endif //MAILBOX_H
