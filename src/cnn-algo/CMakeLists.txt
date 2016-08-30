cmake_minimum_required(VERSION 3.5)
project(cnn_algo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    cnn.c
    cnn.h
    cnn_tb.c
    glob.h)

add_executable(cnn_algo ${SOURCE_FILES})