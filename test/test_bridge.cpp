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

TEST(BridgeTest, InitialiseMqTest) {
        std::string mq_path = "/test_mq";
        int rc = init_mq(mq_path.c_str());
        ASSERT_NE(initialize_mq(mq_path.c_str()), -1);
        remove_mq(mq_path.c_str());
}

TEST(BridgeTest, ConnectToBrokerTest) {
        MQTTAsync client = init_MQTT_client();
        ASSERT_EQ(connect_to_broker(&client), MQTTASYNC_SUCCESS);
}