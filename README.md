## 1. Setup

  **Remark**: *This guide is only for Linux, specifically Ubuntu and other Debian-based distributions*
  
### Clone the repository

```bash
git clone git@gitlab.ibr.cs.tu-bs.de:cm-courses/sep/groups/ss24/cm0/stats-recording.git
```

### Install Linux Developer Tools

- Debian, Ubuntu, popOS, and other Debian-based distributions:
  
```bash
sudo apt-get update && apt-get upgrade -y
sudo apt-get install build-essential tar curl zip unzip
sudo apt-get install libsensors4-dev  # for cyclic-file-read
```

### Install paho-mqtt library

```bash
git clone git@github.com:eclipse/paho.mqtt.c.git
cd paho.mqtt.c
mkdir build
cd build
cmake ..
make
sudo make install
```

## 2. Build and Run

- Build:
  
```bash
mkdir build   # if not already exists
cd build
cmake ..
make
```

- Run:
```bash
./bridge-exec
```

Alternatively use the **CMake-Extension** in VSCode:
- Open the Command Palette and try following commands:
  * *CMake:Build*
  * *CMake:Debug*

## 3. Testing

### Inotify-coredump

- This component gets notified, whenever a coredump is created in `/var/lib/systemd/coredump`
- First build the project
- For the following, you will need 3 terminals in `stats-recording`:

  **Terminal 1**:

  ```bash
  cd build
  ./bridge-exec
  ```

  **Terminal 2**:

  ```bash
  cd build/Testing
  ./inotify-coredump-exec
  ```

  **Terminal 3**:

  ```bash
  cd testing/create_coredumps
  gcc -o segmentation_fault segmentation_fault.c
  ./segmentation_fault
  ```

- Now you should see the output in *Terminal 1* and *Terminal 2*

- For more information [read](/testing/create_coredumps/README.md) the `README.md` in `testing/create_coredumps`

***Good luck*** :sunglasses: