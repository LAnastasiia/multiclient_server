cmake_minimum_required(VERSION 3.15)
project(exam)

set(CMAKE_CXX_STANDARD 17)

add_executable(exam main.cpp services/services.cpp)
add_executable(exam_tester _client.cpp)