//
// Created by guita on 20.06.2025.
//
#include "./mailbox.h"
#include <stdexcept>
#include <iostream>



size_t Mailbox::getIndexToInsert(const event_t *event) {
    for (size_t i = mailbox.size() - 1; i > 0; --i) { //TODO Fix this function
        event_t* other_event = mailbox[i];
        if (other_event == nullptr) {
            throw std::invalid_argument("There shouldn't be a nullptr in mailbox");
        }
        bool event_has_higher_priority = priorityManager.has_higher_priority(event, other_event);
        if (!event_has_higher_priority) {
            return i; // Insert after this element
        }
    }
    return mailbox.size()-1; // For now, just append to the end
}
bool Mailbox::mail(event_t *event) {

    if (!event) {
        throw std::invalid_argument("Event pointer cannot be null");
    }
    if (mailbox.size() >= MAX_MAILBOX_SIZE) {
        std::cerr << "Mailbox is full, cannot add new event." << std::endl;
        return false; // Mailbox is full
    }

    size_t index = getIndexToInsert(event);

    event_t* last_elem = mailbox.at(mailbox.size()-1);

    if (last_elem == nullptr || last_elem != event){
        throw std::runtime_error("Failed to insert event into mailbox");
    }

    return true;
}
