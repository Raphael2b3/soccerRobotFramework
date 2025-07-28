//
// Created by guita on 25.07.2025.
//

#ifndef IAGENT_H
#define IAGENT_H

#include "eventhandler/eventhandler.h"
#include "id/id.h"
#include "messaging/mailbox/mailbox.h"


class IAgent
{
public:
    Mailbox mailbox;
    AgentId runtime_id;
    EventHandler event_handler;
    std::atomic<bool> running = true;

    virtual void init()
    {
    };

    virtual void main()
    {
    };

    virtual ~IAgent() = default;


    virtual void kill()
    {
    };

    virtual void pause()
    {
        running = false;
    };

    virtual void resume()
    {
        running = true;
    };

    template <typename T>
    void on(std::function<void(const T&)> callback)
    {
        T::subscribe(this);
        event_handler.register_callback<T>(callback);
    }

    template <typename Event,typename Backend>
    void emit( std::shared_ptr<Event> event)
    {
        event->sender_id = this->runtime_id;
        Event::template emit<Backend>(event);
    }
};
#endif //IAGENT_H
