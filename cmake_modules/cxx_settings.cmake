cmake_minimum_required(VERSION 3.10)
find_package(libpqxx REQUIRED)
set(CMAKE_PREFER_PTHREAD_FLAG ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb  -Wall -std=c++17")
