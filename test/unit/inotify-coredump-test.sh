#!/bin/bash

./../../build/inotify-coredump-exec -t &
sleep 1
gcc -o segmentation_fault ../create_coredumps/segmentation_fault.c
./../create_coredumps/segmentation_fault
killall inotify-coredump-exec
