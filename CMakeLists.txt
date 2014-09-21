cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS}  -g")

SET(CMAKE_BUILD_TYPE Debug)

project(kvstore CXX C)

# Look for flags to enable C++11 support, if any.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_HAS_STD_CXX11)
if (COMPILER_HAS_STD_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_HAS_STD_CXX0X)
    if (COMPILER_HAS_STD_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
endif()

include_directories(./include/)

link_directories(/usr/lib)

set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(
  client  
  src/main_client.cc
  src/Client.cc
  src/Message.cc
  )

add_executable(
  server
  src/main_server.cc
  src/KVServer.cc
  src/Message.cc
  )

target_link_libraries(
  server
  sqlite3
  )
