cmake_minimum_required(VERSION 3.18)
project(cpp_httplib_dev)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent
include(FetchContent)

# Set custom base dir for FetchContent
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")

# ----------- External Dependencies ------------

# cpp-httplib (header-only)
FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.20.0
    SOURCE_SUBDIR "non_existent_directory"  # prevents auto CMakeLists
)

# bcrypt
FetchContent_Declare(
    bcrypt
    GIT_REPOSITORY https://github.com/trusch/libbcrypt.git
    GIT_TAG master
)

# jwt-cpp
FetchContent_Declare(
    jwt_cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG master
)

# Fetch all above
FetchContent_MakeAvailable(cpp_httplib bcrypt jwt_cpp)

# nlohmann_json should be installed system-wide
find_package(nlohmann_json 3.12.0 REQUIRED)

# ----------- Source Files ------------

set(SOURCE_DIRS models handlers utils services)

set(SOURCES main.cpp)
foreach(dir ${SOURCE_DIRS})
    file(GLOB_RECURSE ${dir}_sources ${dir}/*.cpp)
    list(APPEND SOURCES ${${dir}_sources})
endforeach()

add_executable(server ${SOURCES})

# ----------- Include Paths ------------
set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake")

foreach(dir ${SOURCE_DIRS})
    target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/${dir})
endforeach()

target_include_directories(server PRIVATE
    ${FETCHCONTENT_BASE_DIR}/cpp_httplib-src
    ${bcrypt_SOURCE_DIR}/include
    ${jwt_cpp_SOURCE_DIR}/include
)
# ----------- Find & Link Libraries ------------

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(bsoncxx REQUIRED)
find_package(mongocxx REQUIRED)

include_directories(${MONGOCXX_LIBRARIES})
include_directories(${BSONCXX_LIBRARIES})
include_directories(${MONGOCXX_INCLUDE_DIRS})
include_directories(${BSONCXX_INCLUDE_DIRS})

include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/libbson-1.0")
include_directories("/usr/local/lib")

target_link_options(server PRIVATE "-Wl,-rpath,/usr/local/lib")

target_link_libraries(server
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        OpenSSL::SSL
        OpenSSL::Crypto
        bcrypt
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
    PUBLIC
        nlohmann_json::nlohmann_json
)
