#include "inotify-coredump.h"
#include "line_protocol_parser.h"
#include <errno.h>
#include <unistd.h>
#include <unity.h>

void setUp(void) {
}
void tearDown(void) {
}
// Mock read function
ssize_t mock_read_failure(int fd, void *buf, size_t count) {
        errno = EINTR;
        return -1; // Simulate an error due to interrupt
}

void test_init_inotify_coredump(void) {
        int fd = -1, wd = -1;
        TEST_ASSERT_EQUAL_INT8(0, init_inotify(&fd, &wd));
        cleanup(fd, wd);
}

void test_line_protocol(void) {
        char message[1024];
        coredump_to_line_protocol(message, "test.coredump.dump");
        TEST_ASSERT_EQUAL_INT(0, LP_check(message));
}
/*
void test_process_events_read_failure(void) {
        // Save the original read function pointer
        ssize_t (*real_read)(int, void *, size_t) = read;
        // Override the read function pointer with the mock read function
        read = mock_read_failure;

        // Call process_events and expect -1 due to mocked read failure
        assert_int_equal(process_events(0, 0, 0), -1);

        // Reset the read function pointer to the real read function
        read = real_read;
}
*/

int main(void) {
        UNITY_BEGIN();

        RUN_TEST(test_init_inotify_coredump);
        RUN_TEST(test_line_protocol);

        return UNITY_END();
}