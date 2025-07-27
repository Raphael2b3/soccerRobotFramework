#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "id/id.h"

TEST_CASE("AgentId equality and inequality operators") {
    AgentId::setHostId("localhost");

    AgentId id1(1, "AgentX", "localhost");
    AgentId id2(1, "AgentX", "localhost");
    AgentId id3(2, "AgentY", "localhost");

    CHECK(id1 == id2);
    CHECK_FALSE(id1 != id2);
    CHECK(id1 != id3);
    CHECK_FALSE(id1 == id3);
}

TEST_CASE("AgentId::getNewId returns unique ids") {
    AgentId::setHostId("127.0.0.1");

    auto id1 = AgentId::getNewId("TestAgent");
    auto id2 = AgentId::getNewId("TestAgent");

    CHECK(id1 != id2);
    CHECK(id1.agent_name == "TestAgent");
    CHECK(id1.host_id == "127.0.0.1");
    CHECK(id2.agent_number == id1.agent_number + 1);
}

TEST_CASE("AgentId::toString formats correctly") {
    AgentId id(42, "MyAgent", "192.168.1.1");
    CHECK(id.toString() == "AgentId(42, MyAgent, 192.168.1.1)");
}

TEST_CASE("AgentId::operator< provides total ordering") {
    AgentId a1(1, "A", "host1");
    AgentId a2(2, "A", "host1");
    AgentId b1(1, "B", "host1");
    AgentId h2(1, "A", "host2");

    CHECK(a1 < a2);
    CHECK(a1 < b1);
    CHECK(a1 < h2);
    CHECK_FALSE(h2 < a1);
}

TEST_CASE("AgentId is hashable") {
    std::unordered_map<AgentId, std::string> map;
    AgentId id(5, "AgentHash", "host");

    map[id] = "hello";

    CHECK(map.find(id) != map.end());
    CHECK(map[id] == "hello");
}