#!/bin/bash

printf "Starting Unit test for inotify-coredump...\n\n"

# Start inotify-coredump in test mode
./../../build/inotify-coredump-exec -t -v &
INOTIFY_PID=$!
sleep 1
ps $INOTIFY_PID

# Create coredump file
echo "Generating coredump..."

sleep 500 &
kill -s SIGTRAP $(pgrep sleep)

# Create a simple C program that causes a segmentation fault
cat <<EOF >segmentation_fault.c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *p = NULL;
    *p = 42;  // This line causes a segmentation fault
    return 0;
}
EOF

# Compile the C program
gcc -o segmentation_fault segmentation_fault.c

# Run the segmentation fault program to generate a coredump
echo "Creating segmentation fault to generate coredump..."
set +e
./segmentation_fault
set -e

ps $INOTIFY_PID

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
