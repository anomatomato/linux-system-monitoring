#include "fixture/InotifyCoredumpFixture.h"
#include "mock/MockMQ.h"
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <mqueue.h>
#include <sys/inotify.h>

extern "C" {
#include "line_protocol_parser.h"
#include "mq.h"
}

/* Initialize the mock instance */
MockMQ *mock_mq = nullptr;

void read_file_into_buffer(const char *filename, char *buffer, size_t buffer_size) {
        FILE *file = fopen(filename, "r");
        if (!file) {
                perror("Failed to open file");
                exit(EXIT_FAILURE);
        }

        size_t bytesRead = fread(buffer, 1, buffer_size, file);
        if (bytesRead == 0 && ferror(file)) {
                perror("Failed to read from file");
                fclose(file);
                exit(EXIT_FAILURE);
        }

        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        fclose(file);
}


TEST(InotifyCoredumpInit, InotifyCoredumpInitSuccess) {
        struct coredump_monitor_t monitor = { 0 };
        ASSERT_EQ(init_inotify(&monitor), 0);
}

TEST_F(InotifyCoredumpFixture, CoredumpToLineProtocol) {
        char message[] = "test_coredump";
        EXPECT_GE(coredump_to_line_protocol(buffer, message), 0);
        EXPECT_EQ(LP_check(buffer), 0);
}

TEST_F(InotifyCoredumpFixture, SendCoredumpSucess) {
        MockMQ mq;
        mock_mq = &mq;

        /* Mock function */
        EXPECT_CALL(mq, send_to_mq(testing::_, testing::_)).WillOnce(testing::Return(0));

        read_file_into_buffer("data/coredump_input.txt", buffer, sizeof(buffer));
        struct inotify_event *event = (struct inotify_event *) buffer;
        EXPECT_NE(event, nullptr);

        EXPECT_GE(event->mask & IN_CREATE, 0);
        EXPECT_EQ(send_coredump(buffer, 96, &monitor), 0);

        mock_mq = nullptr;
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
