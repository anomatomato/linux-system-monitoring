## 1. Setup

### Clone the repository

```bash
git clone git@gitlab.ibr.cs.tu-bs.de:cm-courses/sep/groups/ss24/cm0/stats-recording.git
```

### Install Linux Developer Tools

- Debian, Ubuntu, popOS, and other Debian-based distributions:
  
```bash
sudo apt-get update && apt-get upgrade -y
sudo apt-get install build-essential tar curl zip unzip
```

### Install vcpckg
- Official repository: https://github.com/microsoft/vcpkg
- For the following commands, you might need to use `sudo`, e.g. in `/opt`
- First download and bootstrap vcpkg itself in an open terminal:

```bash
cd /opt
sudo git clone https://github.com/microsoft/vcpkg
sudo ./vcpkg/bootstrap-vcpkg.sh
```

- Next install the `paho-mqtt` library:

```bash
sudo ./vcpkg/vcpkg install paho-mqtt
```

### VSCode with CMake Tools and vcpkg
- Install the **C/C++ Extension** and the **CMake Extension** by Microsoft
- Open `stats-recording` in vscode with *File>Open Folder* or in terminal:
   
  ```bash
  cd path/to/stats-recording  #insert path/to/
  code .
  ```

- Add the following to your workspace `settings.json` will make CMake Tools automatically use vcpkg for libraries:

```json
{
  "cmake.configureSettings": {
    "CMAKE_TOOLCHAIN_FILE": "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake"
  },
  "other.settings":{

  }
}
```

- `settings.json` typically in a `.vscode` folder

Finally configure the include paths. This tells the IntelliSense where to look for header files:

  1. **Access the Command Palette**: Press `F1`, `⇧+⌘+P` or `Ctrl+Shift+P` to open the Command Palette
  2. **Edit Configurations**: 
     1. Type `C/C++:Edit configurations(UI)` scroll down to *Include Path* and insert 
     `/opt/vcpkg/installed/[architecture]/include`. 
        - ***Architecture*** can be e.g. *x64-linux* or *arm-64*
     2. Alternative: Type `C/C++:Edit Configurations (JSON)` and edit the `c_cpp_properties.json` file instead.
Example:
     
      ```json
      "includePath": [
        "${workspaceFolder}/**",
        "/opt/vcpkg/installed/[architecture]/include"
      ]
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
./stats-recording
```

## 3. Testing

### Inotify-coredump

- This component gets notified, whenever a coredump is created in `/var/lib/systemd/coredump`
- First build the project
- For the following, you will need 3 terminals in `stats-recording`:

  **Terminal 1**:

  ```bash
  cd build
  ./stats-recording
  ```

  **Terminal 2**:

  ```bash
  cd build/Testing
  ./inotify-coredump-test
  ```

  **Terminal 3**:

  ```bash
  cd testing/create_coredumps
  ./segmentation_fault
  ```

- Now you should see the output in *Terminal 1* and *Terminal 2*

- For more information read the `README.md` in `testing/create_coredumps`

***Good luck*** :sunglasses: