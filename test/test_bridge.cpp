#include <gtest/gtest.h>

extern "C" {
#include "bridge.h"
#include "unistd.h"
}

TEST(BridgeTest, AddHostnameToMsgSuccess) {
        char *msg = "weather,location=us-midwest temperature=82,humidity=71 1465839830100400200";
        char* hostname;
        gethostname(hostname, sizeof(hostname));
        add_hostname_to_msg(msg);
        char *str1 = "weather,location=us-midwest,host=";
        strcpy(str1, hostname);
        strcpy(str1, "temperature=82,humidity=71 1465839830100400200")
        ASSERT_STREQ(msg,str1) << "Adding hostname failed";
}