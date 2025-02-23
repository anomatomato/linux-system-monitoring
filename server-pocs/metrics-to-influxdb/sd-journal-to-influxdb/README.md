# Sd-journal-to-influxdb

This component reads journal entries from a journal file in directory /var/log/journal/remote of the docker
container systemd-journal-remote via `sd-journal(3)` and converts it in the line protocol. Then journal logs will be
sent to influxdb via MQTT.

In order to directly read journal files in directory /var/log/journal/remote of the docker container
systemd-journal-remote, this component will also be run on a single docker container called metrics-to-influxdb
( together with the component coredump-to-influxdb ). In there, two docker containers `metrics-to-influxdb` and
`systemd-journal-remote` share the same volumn systemd-journal-data and its value is /var/log/journal/remote .

## Build and Run

**Remark**: *This guide is only for Linux, specifically Ubuntu and other Debian-based distributions*

1. Clone the repository

    ```bash
    git clone git@gitlab.ibr.cs.tu-bs.de:cm-courses/sep/groups/ss24/cm0/server-pocs.git
    ```

2. Set up and run all containers
    All necessary containers needs to be started such as influxdb, telegraf, grafana,
    systemd-journal-remote, debian-package-repo
    [For more details](../../README.md)

3. run the code

    -    go into the metrics-to-influxdb container with following command:

            ```bash
            docker exec -it metrics-to-influxdb bash
            ```

    -    type

            ```bash
            sd-journal-to-influxdb
            ```

## Note

If all neccessary containers are set up (step 1 and 2 are done) and you want to extend the source code of this
component, you need to stop, start and rerun the container metrics-to-influxdb so that your change is applied in
the executable file (as people usually run `cmake ..`, `make` and  `./programm`  in the build order). In order to
compact the processes above, you can use the predefined **task.json** to run `metrics-to-influxdb` faster.

- Type  `Ctrl+Shift+P` to open the Command Palette
- On the Command Palette type `run tasks` then
- choose  `attach to metrics-to-influxdb`

During the task is executed, you may encouter the following question


    WARNING! This will remove all dangling images.
    Are you sure you want to continue? [y/N]

Type `y` to remove dangling (unnamed) images. After the executation you should directly go into
the container metrics-to-influxdb. Then run `sd-journal-to-influxdb` to activate the component.

If you want to terminate one specific task, press  `Ctrl+Shift+P`, type`terminate tasks` and choose the task you
want to terminate.
