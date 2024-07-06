# Inotify-Pacct

This component enables the process accounting and saves the report of terminated processes in the file 
/var/log/pacct/acct via `acct(2)`. The modification of this file will be monitored via `inotify(7)` then the 
relevant terminated processes will be extracted via `acct(5)`, modified in line protocol and sent to message queue, 
where they will be taken by the component `poxis-mq-to-mqtt-bridge` and sent to Influxdb via MQTT.

## Build 

> **Note**: Before running inotify-pacct, you have to make sure 
> - the component `poxis-mq-to-mqtt-bridge` is running.
> - the acct file exists on your system. 
>   - run `sudo mkdir -p /var/log/pacct` on your terminal to ensure the folder pacct exists, then 
>   - go to the folder pacct `cd /var/log/pacct`, list all files and directories of this folder `ls -l`
>     - If the file exists, `acct` should be listed in white colour because it should be a normal file.
>     - If the file doensn't exist, run `sudo touch acct` to create one.

You can read the detailed information for building the components [here](../../README.md)

## Normal run

navigate to ./build directory and run:

```bash
./inotify-pacct-exec
```
- All metrics are collected and sent to message queue without printing out to terminal.

## with flags

```bash
./cyclic-file-read-exec <flag>
```

### -v

- Verbose mode: print the message in Line Protocol to the message queue

### -h
- print help message

## Note

After running the system in several months or years, your acct file may contain a greate number of information.
If you are sure that resetting the acct file don't cause the loss of needed information, you can use the predefined
**tasks.json** to do it, type  `Ctrl+Shift+P` then `reset acct file`.
