cmake_minimum_required(VERSION 3.10)
project(cpp_httplib_dev)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define external directories
set(EXTERNAL_DIR "${CMAKE_BINARY_DIR}/external")
set(HTTPLIB_HEADER "${EXTERNAL_DIR}/httplib.h")
set(JSON_URL "https://github.com/nlohmann/json/releases/latest/download/json.hpp")
set(JSON_HEADER "${EXTERNAL_DIR}/json.hpp")

# Create external directory if it doesn't exist
file(MAKE_DIRECTORY ${EXTERNAL_DIR})

# Download the latest httplib.h
file(DOWNLOAD
    https://github.com/yhirose/cpp-httplib/releases/latest/download/httplib.h
    ${HTTPLIB_HEADER}
    SHOW_PROGRESS
)

file(DOWNLOAD
    ${JSON_URL}
    ${JSON_HEADER}
    SHOW_PROGRESS
)

# Define source directories
set(SOURCE_DIRS controllers utils tests)

# Initialize an empty list for source files
set(SOURCES main.cpp)

# Loop through each source directory and collect all .cpp files
foreach(dir ${SOURCE_DIRS})
    file(GLOB_RECURSE ${dir}_sources ${dir}/*.cpp)
    list(APPEND SOURCES ${${dir}_sources})
endforeach()

# Add the executable with all source files
add_executable(server ${SOURCES})

# Include directories
include_directories(${SOURCE_DIRS})

# Include the external directory
include_directories(${EXTERNAL_DIR})
