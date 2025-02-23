#include "fixtures.h"
#include "utils.h"
#include <thread>
#include <chrono>

extern "C" {
#include "sd_journal_to_influxdb.h"
}

TEST_F(ConnectToMQTTFixture, ConnectToBrokerTest) {
        std::cout << "Start test connecting to mqtt\n";
        mqtt_config_t config = {
                .host = "tcp://localhost:1883/",
                .port = 1883,
                .topic = "linux-monitoring/logs/sd_journal",
                .client_id = "connect_to_mqtt_test",
                .qos = 1,
                .timeout = 10000,
        };
        int result = init_mqtt(&config);
        EXPECT_EQ(result, 0);
        close_mqtt(&config);
}

TEST_F(PublishToMQTTFixture, PublishToMQTTTest) {
        std::cout << "\nStart test publishing to mqtt\n";
        mqtt_config_t config = {
                .host = "tcp://localhost:1883/",
                .port = 1883,
                .topic = "linux-monitoring/logs/sd_journal",
                .client_id = "publish_to_mqtt_test",
                .qos = 1,
                .timeout = 10000,
        };
        char msg[] = "Hello, World!";
        if (init_mqtt(&config) != MQTTCLIENT_SUCCESS) {
                FAIL()<< "Connection was not established succesfully";
                close_mqtt(&config);
        };
        publish_mqtt(&config, msg);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::string result = readFile("./result_test2.txt");
        EXPECT_EQ(result, "Hello, World!\n");
        close_mqtt(&config);
}

TEST_F(SdToInfluxdbFixture, JournalToLineProtocolTest) {
    std::cout << "Start test converting journal to line protocol \n";
    pid_t pid = runCommand("../src/journal-to-influxdb localhost", "journal_to_influxdb_log_test3.txt");

    std::this_thread::sleep_for(std::chrono::seconds(2));
    system("journalctl --file=./data/filtered_logs.journal -n 1 --output export | /lib/systemd/systemd-journal-remote -o /var/log/journal/remote/mock_pi.journal -");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    kill_process(pid);
    std::string result = readFile("./result_test3.txt");
    std::string result_no_timestamp = stripTimestampField(result);
    EXPECT_EQ(result_no_timestamp, "sd-journal,journal_time=Jun-22-23:14:31,pi_name=pi-85cd,service=systemd message=\"systemd-coredump@18-41860-0.service: Deactivated successfully.\"\n");
}
