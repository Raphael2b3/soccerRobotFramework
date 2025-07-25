#ifndef ID_H
#define ID_H

#include <string>
#include <atomic>
#include <functional>
#include <sstream>
#include <utility>

class AgentId
{
    inline static std::atomic<int> id_counter = 0;
    inline static std::string this_host_id = "ip:?";

public:
    int agent_number;
    std::string agent_name;
    std::string host_id;

    AgentId(int num, std::string name, std::string host)
        : agent_number(num), agent_name(std::move(name)), host_id(std::move(host))
    {
    }
    AgentId() =default;

    bool operator==(const AgentId& other) const
    {
        return agent_number == other.agent_number
            && agent_name == other.agent_name
            && host_id == other.host_id;
    }

    bool operator!=(const AgentId& other) const
    {
        return !(*this == other);
    }

    static AgentId getNewId(const std::string& name_of_agent)
    {
        return AgentId{id_counter++, name_of_agent, this_host_id};
    }

    static void setHostId(const std::string& id)
    {
        this_host_id = id;
    }

    bool operator<(const AgentId& other) const {
        if (host_id != other.host_id)
            return host_id < other.host_id;
        if (agent_name != other.agent_name)
            return agent_name < other.agent_name;
        return agent_number < other.agent_number;
    }

    [[nodiscard]] std::string toString() const
    {
        std::ostringstream oss;
        oss << "AgentId(" << agent_number << ", " << agent_name << ", " << host_id << ")";
        return oss.str();
    }
};

// Hash function specialization for unordered_map/set
namespace std
{
    template <>
    struct hash<AgentId>
    {
        size_t operator()(const AgentId& id) const noexcept
        {
            return hash<int>()(id.agent_number) ^
                (hash<std::string>()(id.agent_name) << 1) ^
                (hash<std::string>()(id.host_id) << 2);
        }
    };
}

#endif // ID_H
