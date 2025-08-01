//
// Created by guita on 20.06.2025.
//

#ifndef MAILBOX_H
#define MAILBOX_H
#include <deque>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include "messaging/priority/priority.h"

#define MAX_MAILBOX_SIZE 1024

class Mailbox {
    Priority priorityManager;
    std::mutex mailbox_mutex;
    size_t getIndexToInsert(const std::shared_ptr<BaseEvent>& event);

public:
    int max_mailbox_size = MAX_MAILBOX_SIZE;
    std::deque<std::shared_ptr<BaseEvent>> mailbox; // Public for testability

    bool mail(const std::shared_ptr<BaseEvent>& e);

    std::shared_ptr<BaseEvent> getNextEvent();
};
#endif //MAILBOX_H
