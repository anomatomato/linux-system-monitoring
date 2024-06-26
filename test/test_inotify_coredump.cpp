#include <gtest/gtest.h>
extern "C" {
#include "inotify-coredump.h"
#include "line_protocol_parser.h"
}

TEST(inotify_coredump_tests, init_inotify_success) {
        coredump_monitor_t monitor;
        EXPECT_EQ(init_inotify(&monitor), 0);
        cleanup(&monitor);
}
