cmake_minimum_required(VERSION 3.22.1)
project(downhillnative)

add_library(downhillnative SHARED native_runtime.cpp)

find_library(log-lib log)
find_library(gles-lib GLESv3)
target_link_libraries(downhillnative ${log-lib} ${gles-lib})
