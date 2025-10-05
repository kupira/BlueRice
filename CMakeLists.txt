cmake_minimum_required(VERSION 3.16)
project(BlueRice VERSION 0.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -pthread")

# --- Sources ---
file(GLOB SOURCES src/*.cpp)

add_executable(bluerice ${SOURCES})

target_include_directories(bluerice PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Dependencies ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
pkg_check_modules(BLUEZ REQUIRED bluez)

target_include_directories(bluerice PRIVATE ${GLIB2_INCLUDE_DIRS} ${LIBNOTIFY_INCLUDE_DIRS} ${BLUEZ_INCLUDE_DIRS})
target_link_libraries(bluerice PRIVATE ${GLIB2_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${BLUEZ_LIBRARIES})

install(TARGETS bluerice DESTINATION /usr/bin)

