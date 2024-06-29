#ifndef MOCK_MQ_H
#define MOCK_MQ_H

#include <gmock/gmock.h>

class MockMQ {
    public:
        MOCK_METHOD(int, send_to_mq, (const char *, const char *) );
};

/* Declare an instance of the mock class */
extern std::unique_ptr< MockMQ > mock_mq;

extern "C" {

int __real_send_to_mq(const char *message, const char *mq_path);

int __wrap_send_to_mq(const char *message, const char *mq_path) {
        if (mock_mq) {
                return mock_mq->send_to_mq(message, mq_path);
        }
        /* Default behavior if mock_mq is not set */
        return 0;
}
}

#endif