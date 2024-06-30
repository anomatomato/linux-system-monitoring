#ifndef MOCK_MQ_H
#define MOCK_MQ_H

#include <gmock/gmock.h>

class MockMq {
    public:
        MOCK_METHOD(int, init_mq, (const char *) );
        MOCK_METHOD(int, send_to_mq, (const char *, const char *) );
};


extern std::unique_ptr< MockMq > mock_mq;

extern "C" {

int __real_init_mq(const char *mq_path);
int __real_send_to_mq(const char *message, const char *mq_path);
int __wrap_init_mq(const char *mq_path) {
        if (mock_mq) {
                return mock_mq->init_mq(mq_path);
        }
        return __real_init_mq(mq_path);
}
int __wrap_send_to_mq(const char *message, const char *mq_path) {
        if (mock_mq) {
                return mock_mq->send_to_mq(message, mq_path);
        }
        return __real_send_to_mq(message, mq_path);
}
}
#endif