cmake_minimum_required(VERSION 3.15)
project(iridium_forge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get all C++ Source Files
file(GLOB_RECURSE IRIDIUM_SRC CONFIGURE_DEPENDS src/*.cpp)

# Gzip library
find_package(ZLIB REQUIRED)

# Msgpack library
find_package(msgpack-cxx QUIET)
if (NOT msgpack-cxx_FOUND)
  find_package(msgpack QUIET)
endif()

if (TARGET msgpackc-cxx)
  set(MSGPACK_TARGET msgpackc-cxx)
elseif (TARGET msgpack-cxx)
  set(MSGPACK_TARGET msgpack-cxx)
elseif (TARGET msgpack)
  set(MSGPACK_TARGET msgpack)
else()
  message(FATAL_ERROR "msgpack-c++ not found. Install msgpack-cxx (e.g., via vcpkg or your package manager).")
endif()

# Node headers (from cmake-js)
include_directories(${CMAKE_JS_INC})

# Get node-addon-api include path
execute_process(
  COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

# Include node-addon-api
include_directories(${NODE_ADDON_API_DIR})

# ----------------------
# Shared library for Node.js
# ----------------------
add_library(iridiumForge SHARED
    ${IRIDIUM_SRC}
    node-api.cpp
    shared.cpp
    ${CMAKE_JS_SRC}
)

set_target_properties(iridiumForge PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(iridiumForge PRIVATE ZLIB::ZLIB ${MSGPACK_TARGET})
target_include_directories(iridiumForge PUBLIC ${CMAKE_SOURCE_DIR}/include)

# ----------------------
# Standalone executable
# ----------------------
add_executable(forge
    ${IRIDIUM_SRC}
    main.cpp 
    shared.cpp
)

target_link_libraries(forge PRIVATE ZLIB::ZLIB ${MSGPACK_TARGET})
target_include_directories(forge PUBLIC ${CMAKE_SOURCE_DIR}/include)