#ifndef MOCK_IC_H
#define MOCK_IC_H

#include "inotify-coredump.h"
#include <gmock/gmock.h>
#include <iostream>
#include <unistd.h>

class MockInotifyCoredump {
    public:
        MOCK_METHOD(int, coredump_to_line_protocol, (char *, const char *) );
        MOCK_METHOD(ssize_t, read, (int, void *, size_t));
};

/* Declare an instance of the mock class */
extern std::unique_ptr< MockInotifyCoredump > mock_ic;

extern "C" {

ssize_t __real_read(int fd, void *buf, size_t count);
int __real_coredump_to_line_protocol(char *buffer, const char *coredump_msg);

ssize_t __wrap_read(int fd, void *buf, size_t count) {
        if (mock_ic) {
                return mock_ic->read(fd, buf, count);
        }
        return __real_read(fd, buf, count);
}
int __wrap_coredump_to_line_protocol(char *buffer, const char *coredump_msg) {
        if (mock_ic) {
                std::cout << "Mock coredump_to_line_protocol called" << std::endl;
                return mock_ic->coredump_to_line_protocol(buffer, coredump_msg);
        }
        std::cout << "Real coredump_to_line_protocol called" << std::endl;
        return __real_coredump_to_line_protocol(buffer, coredump_msg);
}
}

#endif