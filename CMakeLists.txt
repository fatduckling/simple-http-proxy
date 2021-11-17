cmake_minimum_required(VERSION 3.18)
project(simple_http_proxy)

set(CMAKE_CXX_STANDARD 20)

# show all errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wclass-memaccess")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(include)

# third-party libraries
include_directories(third-party)
include_directories(third-party/uSockets)

# primary sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# definitions required by uWebSockets
add_definitions(-DWITH_LIBUV=1)
add_definitions(-DLIBUS_NO_SSL=1)

# third-party sources
file(GLOB_RECURSE USOCKETS_SOURCES "third-party/uSockets/*.c" "third-party/uSockets/*.cpp")
file(GLOB_RECURSE LZO_SOURCES "third-party/lzo/*.c")

add_executable(simple_http_proxy ${SOURCES} ${USOCKETS_SOURCES} ${LZO_SOURCES})

target_link_libraries(simple_http_proxy curl pthread z boost_program_options)
