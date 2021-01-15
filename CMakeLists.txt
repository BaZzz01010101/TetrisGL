cmake_minimum_required(VERSION 3.0)
project(TetrisGL)

if(WIN32)
  message( FATAL_ERROR "Use MSVS Project from 'proj/vc14' for Windows." )
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(GLEW REQUIRED)
find_package(glfw3 3.0 REQUIRED)
find_package(ALSA REQUIRED)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CONFIGURATION_TYPES "Release" "Debug")
file(GLOB SRC src/*.cpp src/3rdParty/mm_core/*.cpp)
include_directories("src/3rdParty")
add_executable(TetrisGL ${SRC})
set(EXECUTABLE_OUTPUT_PATH "../bin")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

target_link_libraries(TetrisGL m X11 GL GLU GLEW glfw3 dl pthread asound)

