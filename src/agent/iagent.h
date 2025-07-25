//
// Created by guita on 25.07.2025.
//

#ifndef IAGENT_H
#define IAGENT_H

#include "id/id.h"
#include "messaging/mailbox/mailbox.h"


class IAgent
{
public:
    Mailbox mailbox;
    AgentId runtime_id;
    std::atomic<bool> running = true;

    virtual void init()
    {
    };

    virtual void main()
    {
    };

    virtual ~IAgent() = default;

    /**
     *  Delete this object. Dont use it after killing it.
     * @return
     */
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
    void on(std::function<void(T*)> callback)
    {
        T::subscribe(this, callback);
    }
};
#endif //IAGENT_H
