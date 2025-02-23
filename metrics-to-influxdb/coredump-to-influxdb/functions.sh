#!/bin/bash

# Directory where systemd journal files are located

# On local machine use "/var/log/journal" (testing only)
# JOURNAL_DIR="/var/log/journal"
# On server use "/var/log/journal/remote"
JOURNAL_DIR="/var/log/journal/remote"

# MQTT Broker details
#MQTT_BROKER="localhost"
MQTT_BROKER="sep-cm0-server.ibr.cs.tu-bs.de"
MQTT_TOPIC="linux-monitoring/logs/coredumps"
MQTT_PORT="1883"

# Check if required commands are avaiable
check_dependencies() {
    if ! command -v mosquitto_pub &>/dev/null; then
        echo "mosquitto_pub could not be found. Ensure mosquitto-clients is installed."
        exit 1
    fi

    if ! command -v coredumpctl &>/dev/null; then
        echo "coredumpctl could not be found. Ensure coredumpctl is installed."
        exit 1
    fi
}

# Read coredumps
read_coredumps() {
    coredumpctl --directory "$JOURNAL_DIR" list --no-legend --since "$1" 2>&1
}

# Check if coredumps exist
coredumps_exists() {
    echo "$1" | grep -q "No coredumps found"
    if [ $? -eq 0 ]; then
        echo "No coredumps found"
        return 1
    else
        return 0
    fi
}

# Extract coredump data
extract_coredump_metrics() {
    local delimiter=$'\x1F' # ASCII unit separator
    local hostname=$(hostname -f)
    local pid=$(echo "$1" | awk '{print $5}')
    local sig=$(echo "$1" | awk '{print $8}')
    local time=$(echo "$1" | cut -d' ' -f1-4)
    local executable=$(echo "$1" | awk '{print $10}')
    echo "$hostname$delimiter$pid$delimiter$sig$delimiter$time$delimiter$executable"
}

coredump_to_line_protocol() {
    IFS=$'\x1F' read -r hostname pid signal time executable <<<"$1"
    local message="coredump_metrics,hostname=$hostname pid=$pid,signal=\"$signal\",time=\"$time\",executable=\"$executable\""
    echo "$message"
}

# Function to read metrics from core dumps and send them via MQTT
publish_to_mqtt() {
    mosquitto_pub -t "$MQTT_TOPIC" -h "$MQTT_BROKER" -p "$MQTT_PORT" -m "$1" -d
}
