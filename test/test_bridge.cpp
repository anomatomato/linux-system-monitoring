#include <gtest/gtest.h>
#include <string>
extern "C" {
#include "bridge.h"
#include "mq.h"
#include "unistd.h"
}

TEST(BridgeTest, AddHostnameToMsgSuccess) {
        char hostname[64];
        gethostname(hostname, sizeof(hostname));

        std::string str_hostname = hostname;
        std::string line = "weather,location=us-midwest,host=" + str_hostname +
                        " temperature=82,humidity=71 1465839830100400200";
        const char *assert_str = line.c_str();

        char msg[MAX_MSG_SIZE] = "weather,location=us-midwest temperature=82,humidity=71 1465839830100400200";
        add_hostname_to_msg(msg);
        ASSERT_STREQ(msg, assert_str) << "Adding hostname failed";
}