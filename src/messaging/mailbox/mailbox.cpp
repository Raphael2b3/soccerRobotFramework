//
// Created by guita on 20.06.2025.
//
#include <stdexcept>
#include "messaging/mailbox/mailbox.h"
#include "messaging/event/event.h"

size_t Mailbox::getIndexToInsert(const std::shared_ptr<BaseEvent>& event) {

    for (size_t i = mailbox.size(); i > 0; --i) {
        std::shared_ptr<BaseEvent> other_event = mailbox[i-1];
        if (other_event == nullptr) {
            throw std::invalid_argument("There shouldn't be a nullptr in mailbox");
        }
        bool event_has_higher_priority = priorityManager.has_higher_priority(event, other_event);
        if (!event_has_higher_priority) {
            return i; // Insert before this element
        }
    }
    return 0; // For now, just append to the end
}

bool Mailbox::mail(const std::shared_ptr<BaseEvent>& event) {

    std::lock_guard lock(mailbox_mutex);
    if (!event) {
        throw std::invalid_argument("Event pointer cannot be null");
    }
    if (mailbox.size() >= max_mailbox_size) {
        // std::cerr << "Mailbox is full, cannot add new event." << std::endl;
        return false; // Mailbox is full
    }
    size_t index = getIndexToInsert(event);

    mailbox.insert(mailbox.cbegin() + index, event);

    std::shared_ptr<BaseEvent> last_elem = mailbox.at(index);

    if (last_elem == nullptr || last_elem != event){
        throw std::runtime_error("Failed to insert event into mailbox");
    }

    return true;
}

std::shared_ptr<BaseEvent> Mailbox::getNextEvent()
{
    std::lock_guard lock(mailbox_mutex);
    if (mailbox.empty()) {
        return nullptr;
    }
    auto nextEvent = mailbox.front();
    mailbox.pop_front();  // Efficient removal from front
    return nextEvent;
}
