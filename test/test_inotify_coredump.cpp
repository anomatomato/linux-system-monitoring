#include "fixtures.h"
#include <future>
#include <gtest/gtest.h>
#include <sys/inotify.h>
#include <thread>

extern "C" {
#include "line_protocol_parser.h"
#include "mq.h"
}

void trigger_coredump() {
        /* Ensure core dumps are enabled */
        std::system("ulimit -c unlimited");

        int pid = fork();
        if (pid == 0) {
                execlp("sleep", "sleep", "10", (char *) NULL);
                exit(EXIT_SUCCESS);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::system(("kill -s TRAP " + std::to_string(pid)).c_str());
}

TEST_F(InotifyCoredumpFixture, CoredumpToLineProtocol) {
        char buffer[MAX_MSG_SIZE];
        char message[] = "test_coredump";
        EXPECT_GE(coredump_to_line_protocol(buffer, message), 0);
        EXPECT_EQ(LP_check(buffer), 0);
        // struct inotify_event event = { .mask = IN_CREATE, .len = 0 };
}

TEST_F(InotifyCoredumpFixture, ReceiveCoredump) {
        std::future< void > triggerThread = std::async(std::launch::async, trigger_coredump);

        char buffer[MAX_MSG_SIZE];
        EXPECT_EQ(receive_coredump(buffer, &monitor), 0);

        triggerThread.wait();

        struct inotify_event *event = (struct inotify_event *) buffer;
        EXPECT_EQ(event->mask, IN_CREATE);
}