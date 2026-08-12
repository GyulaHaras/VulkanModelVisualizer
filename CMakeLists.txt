cmake_minimum_required(VERSION 3.10)
project(VulkanModelVisualizer LANGUAGES C CXX)

# Enviromental variables
# Compilation flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Asset paths
set(MODELS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# External Libraries Import
include(FetchContent)

find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(glfw)

# Main Target Source and Executable

add_executable(
    VulkanModelVisualizer
        src/helper/math.h
        # src/vulkan_app/vulkan_app.h
)

target_sources(VulkanModelVisualizer
    PRIVATE
        src/main.cpp
)

# Model Text Parser Library

add_library(ModelTextParser)

target_sources(ModelTextParser
    PRIVATE
        src/ModelTextParser.c
        src/mock.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            src
        FILES
            src/ModelTextParser.h
            src/mock.h
)

# Vulkan App Library

add_library(ModelVisualizerVulkanApp
                src/vulkan_app/vulkan_app.h
                src/vulkan_app/vulkan_app.cpp
)

target_sources(ModelVisualizerVulkanApp
    PRIVATE
        src/vulkan_app/vulkan_app.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            src
        FILES
            src/vulkan_app/vulkan_app.h
)

# swapchain Container Library

add_library(ModelVisualizerSwapchain
                src/swapchain/swapchain.h
                src/swapchain/swapchain.cpp
)

target_sources(ModelVisualizerSwapchain
    PRIVATE
        src/swapchain/swapchain.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            src
        FILES
            src/swapchain/swapchain.h
            src/helper/math.h
)

# Target Library Linking

target_link_libraries(VulkanModelVisualizer
    PRIVATE
        ModelTextParser
        ModelVisualizerVulkanApp
        ModelVisualizerSwapchain
    PUBLIC 
        OpenGL::GL
        Vulkan::Vulkan
        glfw
)

target_link_libraries(ModelVisualizerVulkanApp
    # PRIVATE
    #     ModelVisualizerSwapchain
    PUBLIC
        glm
        OpenGL::GL
        Vulkan::Vulkan
        glfw
)

target_link_libraries(ModelVisualizerSwapchain
    PUBLIC
        OpenGL::GL
        Vulkan::Vulkan
        glfw
)

# Target Compilation
add_compile_definitions(VulkanModelVisualizer 
    PRIVATE
        "MODELS_DIR=\"${MODELS_DIR}\""
        "SHADERS_DIR=\"${SHADERS_DIR}\""
)
target_compile_features(VulkanModelVisualizer PRIVATE cxx_std_20)