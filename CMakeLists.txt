cmake_minimum_required(VERSION 3.15)
project(iridium_forge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
# msgpack-c provides this target name when installed as C++ headers/lib:
# On many systems: find_package(msgpackc-cxx CONFIG REQUIRED)
# Fallback: try plain msgpack
find_package(msgpackc-cxx QUIET)
if (NOT msgpackc-cxx_FOUND)
  find_package(msgpack QUIET)
endif()

if (TARGET msgpackc-cxx)
  set(MSGPACK_TARGET msgpackc-cxx)
elseif (TARGET msgpack)
  set(MSGPACK_TARGET msgpack)
else()
  message(FATAL_ERROR "msgpack-c++ not found. Install msgpack-cxx (e.g., via vcpkg or your package manager).")
endif()

add_executable(iridium_forge src/main.cpp src/Globals.cpp src/IridiumBuildContext.cpp src/IridiumSEXP.cpp)
target_link_libraries(iridium_forge PRIVATE ZLIB::ZLIB ${MSGPACK_TARGET})

# Add include folder
target_include_directories(iridium_forge PUBLIC ${CMAKE_SOURCE_DIR}/include)