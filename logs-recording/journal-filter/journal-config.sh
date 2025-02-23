#!/bin/bash

if [ -f /etc/systemd/coredump.conf ]; then
    if grep -q journal /etc/systemd/coredump.conf; then
        sudo sed -i 's|journal|external|g' /etc/systemd/coredump.conf
    fi

    if grep -q \#Storage=external /etc/systemd/coredump.conf; then
        sudo sed -i 's|#Storage=external|Storage=external|g' /etc/systemd/coredump.conf
    fi
fi

if [ -f /etc/systemd/journal-upload.conf ]; then
    if ! grep -q sep /etc/systemd/journal-upload.conf; then
        sudo sed -i 's|# URL=|URL=http://sep-cm0-server.ibr.cs.tu-bs.de|g' /etc/systemd/journal-upload.conf
    fi
fi

mkdir -p "/var/tmp/logs/"
