#!/bin/bash

./../../build/inotify-coredump-exec -t &
sleep 1

cd ../create_coredumps
gcc -o segmentation_fault segmentation_fault.c
./segmentation_fault
killall inotify-coredump-exec
