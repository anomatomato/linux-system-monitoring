#include "gtest/gtest.h"
#include "utils.h"

class ConnectToMQTTFixture : public testing::Test {
        protected:
                int mqtt_broker_pid;
                void SetUp() override {
                        // Start mqtt broker
                        mqtt_broker_pid = runCommand("mosquitto", "mqtt_broker_log_test1.txt");
                        sleep(1);
                        std::cout << "End ConnectToMQTT fixture\n";
                }

                void TearDown() override {
                        std::cout << "Start teardown ConnectToMQTT\n";
                        // Kill mqtt broker
                        kill_process(mqtt_broker_pid);
                        // kill mosquitto
                        system("(killall mosquitto || true)");
                        system("kill -9 $(lsof -t -i:1883)");
                        system("ps -a");
                }
};

class PublishToMQTTFixture : public testing::Test {
        protected:
                int mqtt_broker_pid, test_mqtt_client_pid;
                void SetUp() override {
                        // Start mqtt broker
                        mqtt_broker_pid = runCommand("mosquitto", "mqtt_broker_log_test2.txt");
                        sleep(1);
                        // Start a test mqtt client
                        std::cout << "Start creating mqtt test client \n";
                        test_mqtt_client_pid = runCommand("mosquitto_sub -h localhost -p 1883 -t \"linux-monitoring/logs/sd_journal\"", "result_test2.txt");
                        std::cout << "End PublishToMQTT fixture\n";
                }

                void TearDown() override {
                        std::cout << "Start teardown PublishToMQTT\n";
                        // Kill mqtt broker
                        kill_process(mqtt_broker_pid);
                        // Kill mqtt client
                        kill_process(test_mqtt_client_pid);
                        // kill mosquitto and mosquitto_sub
                        system("(killall mosquitto || true) && (killall mosquitto_sub || true)");
                        system("kill -9 $(lsof -t -i:1883)");
                        system("ps -a");
                }
};

class SdToInfluxdbFixture : public testing::Test {
        protected:
                int mqtt_broker_pid, test_mqtt_client_pid;
                void SetUp() override {
                        // Prepare sd_journal file
                        system("mkdir /var/log/journal/remote");
                        system("journalctl --file=./data/filtered_logs.journal -n 1 --output export | /lib/systemd/systemd-journal-remote -o /var/log/journal/remote/mock_pi.journal -");
                        system("cd /var/log/journal/remote && ls -a && journalctl --file=mock_pi.journal");
                        // Start mqtt broker
                        mqtt_broker_pid = runCommand("mosquitto", "mqtt_broker_log_test3.txt");
                        sleep(1);
                        // Start a test mqtt client
                        test_mqtt_client_pid = runCommand("mosquitto_sub -h localhost -p 1883 -t \"linux-monitoring/logs/sd_journal\"", "result_test3.txt");
                        system("cat ./result_test3.txt");
                        std::cout << "End converting journal to line protocol test fixture\n";
                }

                void TearDown() override {
                        std::cout << "Start teardown converting journal to line protocol\n";
                        // Delete sd journal file
                        system("rm -rf /var/log/journal/remote/mock_pi.journal");
                        // Kill mqtt broker
                        kill_process(mqtt_broker_pid);
                        // Kill mqtt client
                        kill_process(test_mqtt_client_pid);
                        // kill sd_journal_to_influxdb
                        system("(killall mosquitto || true) && (killall mosquitto_sub || true) && (pkill journal-to-infl || true)");
                        system("ps -a");
                }
};
