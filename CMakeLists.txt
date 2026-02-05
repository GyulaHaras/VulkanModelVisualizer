cmake_minimum_required(VERSION 3.10)
project(VulkanModelVisualizer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(glfw)

add_executable(
    VulkanModelVisualizer
)

target_sources(VulkanModelVisualizer
    PRIVATE
        src/main.cpp
)

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

target_link_libraries(VulkanModelVisualizer
    PRIVATE
        ModelTextParser
)

# target_include_directories(VulkanModelVisualizer PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanModelVisualizer 
                        PRIVATE 
                            OpenGL::GL
                            Vulkan::Vulkan
                            glfw
)
target_compile_features(VulkanModelVisualizer PRIVATE cxx_std_20)