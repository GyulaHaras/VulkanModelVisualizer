### Linux Dependencies
- X11 development packages
  - libx11-dev
  - libxrandr-dev
  - libxinerama-dev
  - libxcursor-dev
  - libxi-dev
- GLFW dev packages
  - libgl1-mesa-dev (Debian/Ubuntu/Min. 64 bit systems only, otherwise: mesa-common-dev. Other distros may use different packages).
  - freeglut3-dev
  - libglfw3-dev
- Vulkan
  - Vulkan SDK installation (don't forget to run the setup_env.sh script for setting up the enviromental variables and adding the SDK to the path).

## Build Instructions

### Prerequisites
- CMake 3.21+
- C++20 compiler
- OpenGL dev packages
- Vulkan SDK

### Build (In-progress)
Add the following script to the ~/.bashrc or ~/.profile
```bash
# Replace <version> by semantic version number or remove subfolder if not planning to use different versions
source ~/vulkan/<version>/setup-env.sh
```
```bash
sudo apt install libglu1-mesa-dev freeglut3-dev libglfw3-dev
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
#TODO: Update real url 
git clone https://github.com/dareka/VulkanModelVisualizer
cd VulkanModelVisualizer
cmake -B build
cmake --build build
```

## Debug build
For debugging purposes a Debug build type flag needs to be included with the cmake command
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
``` 