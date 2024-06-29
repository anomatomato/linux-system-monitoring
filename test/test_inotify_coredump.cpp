#include "fixture/InotifyCoredumpFixture.h"
#include "mock/MockMQ.h"
#include <gtest/gtest.h>
#include <sys/inotify.h>

extern "C" {
#include "line_protocol_parser.h"
#include "mq.h"
}

/* Initialize the mock instance */
std::unique_ptr< MockMQ > mock_mq;

int read_file_into_buffer(const char *filename, char *buffer, size_t buffer_size) {
        FILE *file = fopen(filename, "r");
        if (!file) {
                perror("Failed to open file");
                return -1;
        }

        size_t bytesRead = fread(buffer, 1, buffer_size, file);
        if (bytesRead == 0 && ferror(file)) {
                perror("Failed to read from file");
                fclose(file);
                return -1;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        fclose(file);
        return bytesRead;
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
        mock_mq = std::make_unique< MockMQ >();

        /* Mock function */
        EXPECT_CALL(*mock_mq, send_to_mq(testing::_, testing::_)).WillOnce(testing::Return(0));

        int bytesRead = read_file_into_buffer("data/coredump_input.txt", buffer, sizeof(buffer));
        EXPECT_GE(bytesRead, 0);
        std::cout << "Bytes read: " << bytesRead << std::endl;

        struct inotify_event *event = (struct inotify_event *) buffer;
        EXPECT_NE(event, nullptr);

        EXPECT_GE(event->mask & IN_CREATE, 0);
        EXPECT_EQ(send_coredump(buffer, bytesRead, &monitor), 0);

        mock_mq.reset();
}

TEST_F(InotifyCoredumpFixture, SendCoredumpFailure) {
        mock_mq = std::make_unique< MockMQ >();

        /* Mock function */
        EXPECT_CALL(*mock_mq, send_to_mq(testing::_, testing::_)).Times(0);

        struct inotify_event invalid_event;
        invalid_event.len = 0;
        invalid_event.mask = 0;
        memcpy(buffer, &invalid_event, sizeof(invalid_event));

        EXPECT_EQ(send_coredump(buffer, sizeof(buffer), &monitor), 0);

        mock_mq.reset();
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
