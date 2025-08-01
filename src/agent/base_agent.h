//
// Created by guita on 25.07.2025.
//

#ifndef BASE_AGENT_H
#define BASE_AGENT_H

#include "id/id.h"
#include "messaging/mailbox/mailbox.h"

struct BaseAgent {
    Mailbox mailbox;
    AgentId runtime_id;
    std::atomic<bool> running = true;

    virtual void init() {
    };

    virtual void main() {
    };

    virtual ~BaseAgent() = default;


    virtual void kill() {
    };

    virtual void pause() {
        running = false;
    };

    virtual void resume() {
        running = true;
    };
};
#endif //BASE_AGENT_H
