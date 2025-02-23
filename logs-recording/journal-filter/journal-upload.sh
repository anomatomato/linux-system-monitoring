#!/bin/bash

sleep 1
while [ ! -f /var/tmp/logs/filtered-logs.journal ]; do
    sleep 1
done

if [ ! -f /var/tmp/logs/cursor.txt ]; then
    touch /var/tmp/logs/cursor.txt
    /lib/systemd/systemd-journal-upload -D /var/tmp/logs/ --follow
    exit 0
fi

ucursor=$(</var/tmp/logs/cursor.txt)

/lib/systemd/systemd-journal-upload -D /var/tmp/logs/ --after-cursor=$ucursor --follow
exit 0
