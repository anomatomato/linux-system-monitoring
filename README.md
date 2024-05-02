## 1. Setup

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
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh
```
- We will name `[vcpkg root]` as the directory, where you installed vcpkg
- Next install the `paho-mqtt` library:

```bash
./vcpkg/vcpkg install paho-mqtt
```

### VSCode with CMake Tools and vcpkg
- Install the **C/C++ Extension** and the **CMake Extension** by Microsoft
- Add the following to your workspace `settings.json` will make CMake Tools automatically use vcpkg for libraries:

```json
// typically in a ".vscode" folder
{
  "cmake.configureSettings": {
    "CMAKE_TOOLCHAIN_FILE": "[vcpkg root]/scripts/buildsystems/vcpkg.cmake"
  }
}
```

It should look something like this:

```json
{
  "cmake.configureSettings": {
    "CMAKE_TOOLCHAIN_FILE": "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake"
  },
  // other settings
}
```

Finally configure the include paths. This tells the IntelliSense where to look for header files:
  1. **Open `stats-recording` in vscode** with *File>Open Folder* or in terminal:
   
  ```bash
  cd path/to/stats-recording  #insert path/to/
  code .
  ```

  1. **Access the Command Palette**: Press `F1`, `⇧⌘P` or `Ctrl+Shift+P` to open the Command Palette
  2. **Edit Configurations**: 
     1. Type `C/C++:Edit configurations(UI)` scroll down to *Include Path* and insert 
     `[vcpkg root]/vcpkg/installed/[architecture]/include`. Architecture can be e.g. *x64-linux* or *arm-64*
     2. Alternative: Type `C/C++: Edit Configurations (JSON)` and edit the `c_cpp_properties.json` file instead.
Example:
     
      ```json
      "includePath": [
        "${workspaceFolder}/**",
        "[vcpkg root]/vcpkg/installed/[architecture]/include"
      ]
      ```

