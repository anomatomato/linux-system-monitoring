extern "C" {
#include "inotify-coredump.h"
}
#include "gtest/gtest.h"

class InotifyCoredumpFixture : public testing::Test {
    protected:
        coredump_monitor_t monitor;
        void SetUp() override {
                EXPECT_EQ(init_inotify(&monitor), 0);
        }
        void TearDown() override {
                cleanup(&monitor);
        }
};