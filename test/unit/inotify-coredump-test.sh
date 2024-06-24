#!/bin/bash

# sudo sysctl -w kernel.core_pattern='|/lib/systemd/systemd-coredump %P %u %g %s %t 9223372036854775808 %h' # >/dev/null 2>&1

printf "Starting Unit test for inotify-coredump...\n\n"

# Start inotify-coredump in test mode
./../../build/inotify-coredump-exec -t &
INOTIFY_PID=$!
sleep 2

# Create coredump file
echo "Generating coredump..."

(sleep 500 &) >/dev/null 2>&1
kill -s SIGTRAP $(pgrep sleep) >/dev/null 2>&1

sleep 2

# Wait for inotify-coredump to finish and capture the exit status
wait $INOTIFY_PID
EXIT_STATUS=$?

# Test result
if [ $EXIT_STATUS -ne 0 ]; then
    printf "\nUnit test failed with exit status %s\n" $EXIT_STATUS
    exit $EXIT_STATUS
fi

printf "\nUnit test passed\n"
exit 0
