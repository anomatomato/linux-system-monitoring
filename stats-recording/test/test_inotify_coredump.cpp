#include "fixture/InotifyCoredumpFixture.h"
#include "mock/MockInotifyCoredump.h"
#include "mock/MockMq.h"
#include <gtest/gtest.h>
#include <sys/inotify.h>

extern "C" {
#include "file_utils.h"
#include "inotify-coredump.h"
#include "line_protocol_parser.h"
#include "mq.h"
}

/* Initialize the mock instance */
std::unique_ptr< MockInotifyCoredump > mock_ic;
std::unique_ptr< MockMq > mock_mq;

TEST(InotifyCoredumpInit, InotifyCoredumpInitSuccess) {
        /* Arrange */
        mock_mq = std::make_unique< MockMq >();
        EXPECT_CALL(*mock_mq, init_mq(testing::_)).WillOnce(testing::Return(0));
        struct coredump_monitor_t monitor = { 0 };
        /* Act */
        int result = init_inotify(&monitor);
        /* Assert */
        ASSERT_EQ(result, 0) << "Initializing inotify-coredump failed";

        mock_mq.reset();
}

TEST_F(InotifyCoredumpFixture, CoredumpToLineProtocolSuccess) {
        /* Arrange */
        mock_ic.reset();
        char message[] = "test_coredump";
        /* Act */
        EXPECT_GE(coredump_to_line_protocol(buffer, message), 0);
        /* Assert */
        EXPECT_EQ(LP_check(buffer), 0) << "coredump_to_line_protocol doesn't convert into Line Protocol";
}

TEST_F(InotifyCoredumpFixture, ReceiveCoredumpSuccess) {
        /* Arrange */
        EXPECT_CALL(*mock_ic, read(testing::_, testing::_, testing::_)).WillOnce(testing::Return(0));
        /* Act */
        int result = receive_coredump(buffer, &monitor);
        /* Assert */
        EXPECT_EQ(result, 0) << "receive_coredump failed";
}

TEST_F(InotifyCoredumpFixture, SendCoredumpSucess) {
        /* Arrange */
        // EXPECT_CALL(*mock_ic, coredump_to_line_protocol(testing::_, testing::_)).Times(1);
        EXPECT_CALL(*mock_mq, send_to_mq(testing::_, testing::_)).WillOnce(testing::Return(0));

        int bytesRead = read_file_into_buffer("data/coredump_input.txt", buffer, sizeof(buffer));
        EXPECT_GE(bytesRead, 0);

        struct inotify_event *event = (struct inotify_event *) buffer;
        EXPECT_NE(event, nullptr);
        EXPECT_GE(event->mask & IN_CREATE, 0);
        /* Act */
        // char buf[MAX_MSG_SIZE];
        // coredump_to_line_protocol(buf, "test");
        int result = send_coredump(buffer, bytesRead, &monitor);
        /* Assert */
        EXPECT_EQ(result, 0) << "send_coredump failed";
}

TEST_F(InotifyCoredumpFixture, SendCoredumpInvalid) {
        /* Arrange */
        EXPECT_CALL(*mock_mq, send_to_mq(testing::_, testing::_)).Times(0);
        EXPECT_CALL(*mock_ic, coredump_to_line_protocol(testing::_, testing::_)).Times(0);

        struct inotify_event invalid_event;
        invalid_event.len = 0;
        invalid_event.mask = 0;
        memcpy(buffer, &invalid_event, sizeof(invalid_event));
        /* Act */
        int result = send_coredump(buffer, sizeof(buffer), &monitor);
        /* Assert */
        EXPECT_EQ(result, 0) << "send_coredump failed";
}


/*TEST_F(InotifyCoredumpFixture, ReceiveCoredump) {
        auto triggerThread = std::async(std::launch::async, trigger_coredump);

        char buffer[MAX_MSG_SIZE] = { 0 };
        int result = receive_coredump(buffer, &monitor);
        // auto future = std::async(std::launch::async, [this, &buffer] {
        //         return receive_coredump(buffer, &this->monitor);
        // });
        if (result == -1 && errno == EINTR) {
                ADD_FAILURE() << "receive_coredump was interrupted by timeout.";
        } else {
                EXPECT_EQ(result, 0);
        }
        // if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
        //         ADD_FAILURE() << "receive_coredump timed out (> 5 seconds).";
        // } else {
        //         EXPECT_EQ(future.get(), 0);
        // }

        triggerThread.wait();

        struct inotify_event *event = (struct inotify_event *) buffer;
        if (event) {
                EXPECT_EQ(event->mask & IN_CREATE, IN_CREATE);
        } else {
                ADD_FAILURE() << "Failed to receive inotify event";
        }
}
*/
