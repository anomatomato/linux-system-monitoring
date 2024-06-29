#ifndef INOTIFY_COREDUMP_FIXTURE_H
#define INOTIFY_COREDUMP_FIXTURE_H

extern "C" {
#include "inotify-coredump.h"
#include "mq.h"
}
#include "gtest/gtest.h"

class InotifyCoredumpFixture : public testing::Test {
    protected:
        coredump_monitor_t monitor;
        char buffer[MAX_MSG_SIZE] = { 0 };

        void SetUp() override {
                // mock_read_instance = &custom_read;
                ASSERT_NE(init_inotify(&monitor), -1) << "init_inotify failed";
        }
        void TearDown() override {
                cleanup(&monitor);
                // mock_read_instance = nullptr;
        }
};

#endif