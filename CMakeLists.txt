cmake_minimum_required(VERSION 3.18)
project(cpp_httplib_dev)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent module
include(FetchContent)

# Define the external directory within the source tree
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")

# Declare and fetch cpp-httplib
FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.20.0  # or specify a commit hash or tag
    SOURCE_SUBDIR "non_existent_directory"  # Prevent building tests/examples
)

# Declare and fetch nlohmann/json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)

# Fetch the declared content
FetchContent_MakeAvailable(cpp_httplib json)

# Define source directories
set(SOURCE_DIRS models utils tests)

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
foreach(dir ${SOURCE_DIRS})
    target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/${dir})
endforeach()

# Include the external directory
target_include_directories(server PRIVATE ${FETCHCONTENT_BASE_DIR}/cpp_httplib-src)

find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)