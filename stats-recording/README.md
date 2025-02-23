# Stats-Recording

Raspberry Pis are versatile and powerful devices used in a wide range of applications, from research and education to industrial automation and home projects. 
However, their reliability can be a concern, especially when deployed in remote or harsh environments. Unexpected failures can lead to costly downtime, data loss and inefficient usage.

*Stats-Recording* aims to address these challenges by collecting real-time data on various performance metrics such as CPU, Memory, Network traffic and coredumps with the help of *systemd-services*. The data is then sent to *Mosquitto* on a server, from where it can be graphically shown in *Grafana*. 
By analyzing this data, we can detect anomalies, predict failures and take proactive mesaures to prevent them.


## Contents

- [Stats-Recording](#stats-recording)
  - [Contents](#contents)
  - [Getting started](#getting-started)
    - [Install essential packages](#install-essential-packages)
    - [Setup](#setup)
  - [How to install](#how-to-install)
  - [How to uninstall](#how-to-uninstall)
  - [Build from source](#build-from-source)
  - [Testing](#testing)
  - [How it works](#how-it-works)
  - [Cross compilation](#cross-compilation)
   
## Getting started


### Install essential packages
  
```bash
apt-get install gcc make cmake libsensors4-dev acct systemd-coredump libdbus-1-dev
```

### Setup

1. Verify PSI is enabled by looking for the `/proc/pressure` directory:

```sh
ls /proc/pressure
```

2. Enable PSI (if `/proc/pressure` doesn't exit):
>  1. Edit `/boot/firmware/cmdline.txt` (Pi)
>  2. Add `psi=1` to the kernel command line parameters.
> 
>     ```sh
>     ... existing parameters ... psi=1
>     ```
>
>  3. Reboot your system:
> 
>  ```sh
>  sudo reboot
>  ```
>
>  4. Check if `/proc/pressure` is now available:
> 
>  ```sh
>  ls /proc/pressure
>  ```

[Back to Contents](#contents)

## How to install

There are already debian packages in `debs/`. Check your architecture with:

```sh
dpkg --print-architecture
```

and then install the right package with:

```sh
dpkg -i stats-recording_<version>_<arch>.deb
```

[Back to Contents](#contents)

## How to uninstall

```sh
dpkg -r stats-recording_<version>_<arch>.deb
```

or

```sh
apt remove stats-recording
```

[Back to Contents](#contents)

## Build from source

You can use the `build.sh`, which builds a debian package into `debs/` and builds with CMake in `build-<arch>`:
  
```bash
./build.sh
```

## Testing

Test code is available in the `test` directory. The tests can be built and run by using CMake option `-DSTATS_ENABLE_TESTING`:

```sh
cmake -DSTATS_ENABLE_TESTING=ON -B build
cmake --build build
cd build
ctest -V
```
[Back to Contents](#contents)

## How it works

The stats-collection is divided into multiple components. You can decide on which components to enable:

- **posix-mq-to-mqtt-bridge**: The bridge between *all* the other components and the server. ***Start posix-mq-to-mqtt-bridge first***
- **inotify-coredump**: Monitor `/var/lib/systemd/coredump` for quickly notifying `posix-mq-to-mqtt-bridge` about a
core dump.
- **inotify-pacct**: Send relevant information about terminated processes to `posix-mq-to-mqtt-bridge`
- **cyclic-sysinfo**: Cyclic reading of statistics reported by `sysinfo(2)` and seding those to `posix-mq-to-mqtt-bridge`.
- **cyclic-file-read**: Send stats about CPU, network, disks, proccesses und chip temperature from `proc/` and `sys/` to `posix-mq-to-mqtt-bridge`.
- **epoll-psi**: Send pressure stall information (PSI) to `posix-mq-to-mqtt-bridge`.
- **dbus-systemd-signals**: Send systemd events to `posix-mq-to-mqtt-bridge`.

Starting a component works like with every other systemd-service, e.g.:

```sh
systemctl start posix-mq-to-mqtt-bridge
```


You can find more detailed documentation in the manpage `stats-recording(8)`.

[Back to Contents](#contents)

## Cross compilation

Cross compilation using CMake is performed by using so called "toolchain files" , see `cmake-toolchains(7)`.

The path to the toolchain file can be specified by using CMake's `-DCMAKE_TOOLCHAIN_FILE` option. In case no toolchain file is specified, the build is performed for the native build platform.

For your convenience toolchain files for the following platforms can be found in the `cmake` directory:
- Linux amd64
- Linux arm64
- Linux armhf

Example invocation for the Raspberry Pi:

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.arm64.cmake -B build
```

You can compile and build a debian package for all 3 architectures, for that you will need to install `crossbuild-essential-arm64`
and `crossbuild-essential-armhf`: 

```sh
./build.sh --all
```

Or you can use the `Dockerfile`:

```sh
docker build -t stats-recording-builder .  
docker run --rm -v $(pwd):/app stats-recording-builder
```



***Good luck*** :sunglasses: