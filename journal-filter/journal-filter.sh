#!/bin/bash

bash journal-config.sh

if [ ! -f /var/tmp/logs/filtered-logs.journal ]; then

    journalctl -u ssh.service -u dbus.service -u networking.service -u systemd-coredump@*.sevice -o export -f | /lib/systemd/systemd-journal-remote -o /var/tmp/logs/filtered-logs.journal - | bash journal-upload.sh

else
    oldcursor=$(journalctl -D /var/tmp/logs/ -n 0 --show-cursor | tail -1 | cut -f2- -d: | sed 's/ /"/;s/$/"/' | tr -d '"')
    newcursor=$oldcursor

    while [ "$oldcursor" = "$newcursor" ] ; do
        journalctl --after-cursor=$oldcursor -u ssh.service -u dbus.service -u networking.service -u systemd-coredump@*.sevice -o export | /lib/systemd/systemd-journal-remote -o /var/tmp/logs/filtered-logs.journal -
        newcursor=$(journalctl -D /var/tmp/logs/ -n 0 --show-cursor | tail -1 | cut -f2- -d: | sed 's/ /"/;s/$/"/' | tr -d '"')
        sleep 5
    done

    echo $newcursor > /var/tmp/logs/cursor.txt
    journalctl --after-cursor=$newcursor -u ssh.service -u dbus.service -u networking.service -u systemd-coredump@*.sevice -o export -f | /lib/systemd/systemd-journal-remote -o /var/tmp/logs/filtered-logs.journal - | bash journal-upload.sh

    exit 0
fi

#-u sys-devices-platform@*.device -u networking.service -u systemd@*.sevice
