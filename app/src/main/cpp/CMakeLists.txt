cmake_minimum_required(VERSION 3.22.1)
project(downhillnative)

add_library(downhillnative SHARED native_runtime.cpp)

find_library(log-lib log)
target_link_libraries(downhillnative ${log-lib})
