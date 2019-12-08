cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)
project(wk VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# easylogging++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_LOG_TO_FILE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_DEFAULT_LOG_FILE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_LOGGING_FLAGS_FROM_ARG")

add_subdirectory(libs/CLI11)
add_subdirectory(libs/fmt)
add_subdirectory(libs/nlohmann_json)

include_directories("${wk_SOURCE_DIR}/libs/CLI11/include")
include_directories("${wk_SOURCE_DIR}/libs/fmt/include")
include_directories("${wk_SOURCE_DIR}/libs/easyloggingpp/src")
add_executable(wk src/wk.cpp)
target_sources(wk
  PUBLIC
  src/wk.hpp
  src/add.cpp
  src/delete.cpp
  src/edit.cpp
  src/export.cpp
  src/import.cpp
  src/new.cpp
  src/search.cpp
  libs/easyloggingpp/src/easylogging++.cc
)
target_link_libraries(wk PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(wk PRIVATE fmt::fmt)

