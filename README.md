### Linux Dependencies
- X11 development packages
  - libx11-dev
  - libxrandr-dev
  - libxinerama-dev
  - libxcursor-dev
  - libxi-dev
- GLFW dev packages
  - libgl1-mesa-dev (Debian/Ubuntu/Min. 64 bit systems only, otherwise: mesa-common-dev. Other distros may use different packages).
- Vulkan
  - Vulkan SDK installation

## Build Instructions

### Prerequisites
- CMake 3.21+
- C++20 compiler
- Vulkan SDK

### Build (In-progress)
```bash
TODO: git clone https://github.com/dareka/VulkanModelVisualizer
cd VulkanModelVisualizer
cmake -B build
cmake --build build