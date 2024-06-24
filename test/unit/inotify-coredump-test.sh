#!/bin/bash

sysctl -w kernel.core_pattern='|/lib/systemd/systemd-coredump %P %u %g %s %t 9223372036854775808 %h' # >/dev/null 2>&1

printf "Starting Unit test for inotify-coredump...\n\n"

# Start inotify-coredump in test mode
./../../build/inotify-coredump-exec -t -v &
INOTIFY_PID=$!
sleep 4
ps $INOTIFY_PID

# Create coredump file
echo "Generating coredump..."

sleep 500 &
kill -s SIGTRAP $(pgrep sleep)

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
