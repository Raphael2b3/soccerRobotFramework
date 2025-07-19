//
// Created by guita on 18.07.2025.
//

#ifndef AGENT_H
#define AGENT_H


#include "../messaging/mailbox/mailbox.h"
#include <map>
#include <boost/thread.hpp>

#define agent_id_t int

class Agent {
public:
    Mailbox mailbox;
    agent_id_t runtime_id;
    Agent();

    virtual ~Agent();

    virtual void main();

    bool kill();

    static Agent* spawnNewAgent();

    static Agent* getAgent(agent_id_t id);

private:
    inline static std::map<agent_id_t, Agent*> agent_pool;
    inline static agent_id_t id_counter = 0;
    inline static boost::mutex agent_mutex;

    boost::thread main_thread;
    boost::thread mailbox_handler_thread;
};

#endif //AGENT_H
