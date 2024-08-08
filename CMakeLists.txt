cmake_minimum_required(VERSION 3.10)
project(Hashmap)

set(CMAKE_BUILD_TYPE Debug)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
set(SOURCES
    src/main.c
    src/map.c
    src/crc64.c
    src/blake.c
)

# Create the executable
add_executable(devtool ${SOURCES})

if(UNIX)
    target_compile_options(devtool PUBLIC -Wall -Wextra -pedantic)
endif(UNIX)

target_link_libraries(devtool PRIVATE m)

# target_link_libraries(devtool PRIVATE PkgConfig::GTK4)

# Add includes
target_include_directories(devtool PRIVATE include)
