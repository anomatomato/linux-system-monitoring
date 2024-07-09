#!/bin/bash

echo "Creating coredump..."

sleep 50 &
kill -s SIGTRAP $(pgrep sleep)

coredumpctl info --no-pager
