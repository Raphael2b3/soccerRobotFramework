//
// Created by guita on 18.07.2025.
//
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "messaging/mailbox/mailbox.h"
#include "messaging/event/event.h"

TEST_CASE("Mailbox mail basic insertion") {
    Mailbox mbox;

    std::shared_ptr<BaseEvent> a = std::make_shared<BaseEvent>(AgentId::getNewId("test"),1);
    std::shared_ptr<BaseEvent> b = std::make_shared<BaseEvent>(AgentId::getNewId("test"),2);
    std::shared_ptr<BaseEvent> c = std::make_shared<BaseEvent>(AgentId::getNewId("test"),0);

    CHECK(mbox.mail(a));
    CHECK(mbox.mailbox.size() == 1);
    CHECK(mbox.mailbox[0] == a);

    CHECK(mbox.mail(b));  // Higher priority
    CHECK(mbox.mailbox.size() == 2);
    CHECK(mbox.mailbox[0] == b);  // b inserted before a

    CHECK(mbox.mail(c));  // Lower priority than both
    CHECK(mbox.mailbox.size() == 3);
    CHECK(mbox.mailbox[2] == c);
}

TEST_CASE("Mailbox mail throws on null event") {
    Mailbox mbox;
    CHECK_THROWS_AS(mbox.mail(nullptr), std::invalid_argument);
}

TEST_CASE("Mailbox mail handles full mailbox") {
    Mailbox mbox;
    std::shared_ptr<BaseEvent> dummy = std::make_shared<BaseEvent>(AgentId::getNewId("test"),0);

    for (int i = 0; i < MAX_MAILBOX_SIZE; ++i) {
        CHECK(mbox.mail(std::make_shared<BaseEvent>(AgentId::getNewId("test"),0)));
    }

    CHECK_FALSE(mbox.mail(dummy)); // Should be full now
}

TEST_CASE("Mailbox throws if nullptr exists in mailbox (internal check)") {
    class BrokenMailbox : public Mailbox {
    public:
        void insertNullptrForTest() {
            auto& mb = const_cast<std::vector<std::shared_ptr<BaseEvent>>&>(mailbox);
            mb.push_back(nullptr);
        }
    };

    BrokenMailbox mbox;
    mbox.insertNullptrForTest();
    std::shared_ptr<BaseEvent> new_event =std::make_shared<BaseEvent>(AgentId::getNewId("test"),0);
    CHECK_THROWS_AS(mbox.mail(new_event), std::invalid_argument);
}