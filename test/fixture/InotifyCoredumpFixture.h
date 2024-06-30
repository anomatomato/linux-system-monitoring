#ifndef INOTIFY_COREDUMP_FIXTURE_H
#define INOTIFY_COREDUMP_FIXTURE_H

extern "C" {
#include "inotify-coredump.h"
#include "mq.h"
}
#include "mock/MockInotifyCoredump.h"
#include "mock/MockMq.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern std::unique_ptr< MockInotifyCoredump > mock_ic;
extern std::unique_ptr< MockMq > mock_mq;

class InotifyCoredumpFixture : public testing::Test {
    protected:
        coredump_monitor_t monitor;
        char buffer[MAX_MSG_SIZE] = { 0 };

        void SetUp() override {
                mock_ic = std::make_unique< MockInotifyCoredump >();
                mock_mq = std::make_unique< MockMq >();

                EXPECT_CALL(*mock_mq, init_mq(testing::_)).WillOnce(testing::Return(0));
                ASSERT_NE(init_inotify(&monitor), -1) << "init_inotify failed";
        }
        void TearDown() override {
                cleanup(&monitor);
                mock_ic.reset();
                mock_mq.reset();
        }
};

#endif