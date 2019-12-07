cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(wk VERSION 0.1.0)

add_subdirectory(libs/CLI11)
add_subdirectory(libs/fmt)

include_directories(inc)
include_directories("${wk_SOURCE_DIR}/libs/CLI11/include")
include_directories("${wk_SOURCE_DIR}/libs/fmt/include")
include_directories("${wk_SOURCE_DIR}/libs/easyloggingpp/src")

add_executable(wk src/wk.cpp libs/easyloggingpp/src/easylogging++.cc)
