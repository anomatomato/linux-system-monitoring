#!/bin/bash

. "$(dirname "$0")/functions.sh"

trap cleanup 0 1 2 3 13 15 # EXIT HUP INT QUIT PIPE TERM

# Store last coredump timestamp to avoid duplicate messages
LAST_TIME_FILE=$(mktemp)
last_time='1970-01-01 01:00:00'

cleanup() {
    echo "Received signal, removing $LAST_TIME_FILE"
    rm -rf "$LAST_TIME_FILE"
    # Kill all child processes
    pkill -P $$
    exit 0
}

echo "$last_time" >"$LAST_TIME_FILE"
# Ensure all required commands are available
check_dependencies

while true; do
    echo "Checking for coredumps..."
    # Retrieve coredumps
    last_time=$(cat "$LAST_TIME_FILE")
    coredumps=$(read_coredumps "$last_time")

    # Check if coredumps exist
    if coredumps_exists "$coredumps"; then
        # Process each coredump
        echo "$coredumps" | while read -r line; do
            # Extract metrics
            metrics=$(extract_coredump_metrics "$line")
            # Publish metrics
            message=$(coredump_to_line_protocol "$metrics")
            publish_to_mqtt "$message"
            # Update last coredump timestamp
            echo "$line" | cut -d' ' -f1-4 >"$LAST_TIME_FILE"
            echo "$last_time"
        done
    fi
    # Wait for next cycle
    sleep 10
done
