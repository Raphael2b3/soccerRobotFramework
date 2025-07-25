//
// Created by guita on 18.07.2025.
//
#include "./mailbox.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

TEST_CASE("Mailbox mail basic insertion") {
    Mailbox mbox;

    Event a{0,1,0,nullptr};
    Event b{1,2,0,nullptr};
    Event c{2,0,0,nullptr};

    CHECK(mbox.mail(&a));
    CHECK(mbox.mailbox.size() == 1);
    CHECK(*(mbox.mailbox[0]) == a);

    CHECK(mbox.mail(&b));  // Higher priority
    CHECK(mbox.mailbox.size() == 2);
    CHECK(*(mbox.mailbox[0]) == b);  // b inserted before a

    CHECK(mbox.mail(&c));  // Lower priority than both
    CHECK(mbox.mailbox.size() == 3);
    CHECK(*(mbox.mailbox[2]) == c);
}

TEST_CASE("Mailbox mail throws on null event") {
    Mailbox mbox;
    CHECK_THROWS_AS(mbox.mail(nullptr), std::invalid_argument);
}

TEST_CASE("Mailbox mail handles full mailbox") {
    Mailbox mbox;
    Event dummy{0};

    for (int i = 0; i < MAX_MAILBOX_SIZE; ++i) {
        CHECK(mbox.mail(new Event{i}));
    }

    CHECK_FALSE(mbox.mail(&dummy)); // Should be full now
}

TEST_CASE("Mailbox throws if nullptr exists in mailbox (internal check)") {
    class BrokenMailbox : public Mailbox {
    public:
        void insertNullptrForTest() {
            auto& mb = const_cast<std::vector<Event*>&>(mailbox);
            mb.push_back(nullptr);
        }
    };

    BrokenMailbox mbox;
    mbox.insertNullptrForTest();
    Event new_event{1};
    CHECK_THROWS_AS(mbox.mail(&new_event), std::invalid_argument);
}