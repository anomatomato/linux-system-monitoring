#ifndef MOCK_MQ_H
#define MOCK_MQ_H

#include <gmock/gmock.h>
#include <unistd.h>

class MockInotifyCoredump {
    public:
        MOCK_METHOD(int, send_to_mq, (const char *, const char *) );
        MOCK_METHOD(ssize_t, read, (int, void *, size_t));
};

/* Declare an instance of the mock class */
extern std::unique_ptr< MockInotifyCoredump > mock_ic;

extern "C" {

int __real_send_to_mq(const char *message, const char *mq_path);
ssize_t __real_read(int fd, void *buf, size_t count);

int __wrap_send_to_mq(const char *message, const char *mq_path) {
        if (mock_ic)
                return mock_ic->send_to_mq(message, mq_path);
        return __real_send_to_mq(message, mq_path);
}
ssize_t __wrap_read(int fd, void *buf, size_t count) {
        if (mock_ic)
                return mock_ic->read(fd, buf, count);
        return __real_read(fd, buf, count);
}
}

#endif