cmake_minimum_required(VERSION 3.10)
project(incomsystem)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

include(environment)

add_subdirectory(src)
add_subdirectory(libs)

