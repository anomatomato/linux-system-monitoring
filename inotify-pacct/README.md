# Inotify-Pacct

This component enable the process accounting and save the report of terminated processes in the file /var/log/pacct/acct via `acct(2)`.
The modification of this file will be monitored via `inotify(7)` then the relevant terminated processes will be extracted via `acct(5)`,
modified in line protocol and sent to message queue, where they will be taken by the component `poxis-mq-to-mqtt-bridge` and sent to Influxdb
via MQTT.

# Build and run

> **Note**: Before running inotify-pacct, you have to make sure 
> - the component `poxis-mq-to-mqtt-bridge` is running.
> - the acct file exists on your system. 
>   - run `sudo mkdir -p /var/log/pacct` on your terminal, then 
>   - run `cd /var/log/pacct` and `ls -l`
>     - If the file exists, `acct` should be listed in white colour because it > > > should be a normal file.
>     - If the file doensn't exist, run `sudo touch acct` to create one.

You can read the detailed information for building and running the components [here](../README.md)
