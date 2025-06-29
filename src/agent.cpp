
#include "messaging/mailbox.h"

class Agent: public Mailbox{
public:
    Agent();

    int runtime_id = 0;

    bool kill();

    ~Agent();

private:
    static std::vector<Agent> agent_pool;

    static int id_counter;

    static Agent spawnNewAgent();

    static Agent getAgent(int id);

};

