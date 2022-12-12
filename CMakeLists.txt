cmake_minimum_required(VERSION 3.1...3.25)

project(RayTracing
  VERSION 1.0
  LANGUAGES CXX)
  
add_executable(main src/main.cpp)

find_package(PNGwriter 0.7.0)
if(PNGwriter_FOUND)
  target_link_libraries(main PRIVATE PNGwriter::PNGwriter)
endif(PNGwriter_FOUND)

add_library(threading src/threading/threadpool.cpp src/threading/threadpool.h)

target_link_libraries(main PRIVATE threading pthread)


# C++ settings
set(CMAKE_CXX_STANDARD 11 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-Wall")
