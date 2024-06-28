## 1. Setup

  **Remark**: *This guide is only for Linux, specifically Ubuntu and other Debian-based distributions*
  
### Clone the repository

```bash
git clone git@gitlab.ibr.cs.tu-bs.de:cm-courses/sep/groups/ss24/cm0/stats-recording.git
```

### Install essential packages

- Debian, Ubuntu, popOS, and other Debian-based distributions:
  
```bash
sudo apt-get update && apt-get upgrade -y
sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev
sudo apt-get install libsensors4-dev  # for cyclic-file-read
```

### Install paho-mqtt library
:open_file_folder: [**Link**](https://gitlab.ibr.cs.tu-bs.de/cm-courses/sep/groups/ss24/cm0/documentation/-/blob/main/paho-mqtt.md)

- Now paho-mqtt is stored in `/usr/local/paho-mqtt`

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

- Alternatively use the **CMake-Extension** in VSCode:
  1. Access the Command Palette: Press `F1`, `⇧+⌘+P` or `Ctrl+Shift+P` to open the Command Palette
  2. Try following commands:
     * *CMake:Build*
     * *CMake:Debug*  

- if you even want to save time, you can use the predefined **tasks.json**:
  1. Access the Command Palette: Press `F1`, `⇧+⌘+P` or `Ctrl+Shift+P` to open the Command Palette
  2. If you want to build run a specific component, type `run task`:
     * choose *cmake[all]* to build 
     * choose *run bridge* to activate the bridge
     * choose *run inotify-pacct* to activate inotifying pacct
     * choose *run inotify-coredump* to activate the inotifying coredump
     * choose *run cyclic-file-read* to activate the cyclic-file-read
  3. If you want to deactivate a specific component, type  `Ctrl+Shift+P` then `terminate task`:
     * choose *run bridge* to deactivate the bridge
     * choose *run inotify-pacct* to deactivate inotifying pacct
     * choose *run inotify-coredump* to deactivate the inotifying coredump
     * choose *run cyclic-file-read* to deactivate the cyclic-file-read
  4. If you want to remove build directory, type `remove build directory`
  
- You need [VPN](https://gitlab.ibr.cs.tu-bs.de/cm-courses/sep/groups/ss24/cm0/documentation/-/blob/main/Server.md/#enable-vpn) in order to send stats to the server



## 3. Components

### Inotify-Coredump

- This component gets notified, whenever a coredump is created in `/var/lib/systemd/coredump`
- First build the project
- For Unit Testing run `ctest -V inotify-coredump-test`
- Enable [VPN](https://gitlab.ibr.cs.tu-bs.de/cm-courses/sep/groups/ss24/cm0/documentation/-/blob/main/Server.md/#enable-vpn)
- For the following, you will need 3 terminals in `stats-recording`:

  **Terminal 1**:

  ```bash
  cd build
  ./bridge-exec
  ```

  **Terminal 2**:

  ```bash
  cd build
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

### Inotify-Pacct

- Read [here](/src/inotify-pacct/README.md)

### Cyclic-File-Read

- Read [here](/src/cyclic-file-read/README.md)

***Good luck*** :sunglasses: