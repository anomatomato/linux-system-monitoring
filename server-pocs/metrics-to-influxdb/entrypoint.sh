#!/bin/bash

# Protects script from continuing with an error
set -eu -o pipefail

./coredump-to-influxdb/coredump-to-influxdb.sh &
journal-to-influxdb server
