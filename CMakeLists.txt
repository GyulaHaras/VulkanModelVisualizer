cmake_minimum_required(VERSION 3.10)
project(VulkanModelVisualizer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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