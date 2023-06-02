cmake_minimum_required(VERSION 3.12)
project(cmake_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cmake_test 
    source/main.cpp
)