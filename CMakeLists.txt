cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)
project(wk VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# OSX pre-Catalina? has C++17 language support but LACKS
# some C++17 system library support. Splendid. Work around it.
try_compile(HAS_FS "${CMAKE_BINARY_DIR}/temp" 
"${CMAKE_SOURCE_DIR}/tests/has_filesystem.cpp" 
            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
            LINK_LIBRARIES stdc++fs)
if(HAS_FS)
    message(STATUS "Found std::filesystem support")
else()
    message(STATUS "Missing std::filesystem, using alternative")
    set(USE_ALT_STDFS ON CACHE INTERNAL "")
    add_subdirectory(libs/filesystem)
    include_directories("libs/filesystem/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ALT_STDFS")
endif(HAS_FS)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # -Werror

# easylogging++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_LOG_TO_FILE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_DEFAULT_LOG_FILE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_LOGGING_FLAGS_FROM_ARG")


add_subdirectory(libs/CLI11)
add_subdirectory(libs/fmt)
add_subdirectory(libs/nlohmann_json)
add_subdirectory(libs/SQLiteCpp)
add_subdirectory(libs/yaml-cpp)

include_directories("libs/CLI11/include")
include_directories("libs/fmt/include")
include_directories("libs/easyloggingpp/src")
include_directories("libs/SQLiteCpp/include")
include_directories("libs/yaml-cpp/include")
add_executable(wk src/wk.cpp)
target_sources(wk
  PUBLIC
  src/wk.hpp
  src/defs.hpp
  src/sql.hpp
  src/utils.hpp
  src/sql.cpp
  src/utils.cpp  
  src/add.cpp
  src/delete.cpp
  src/edit.cpp
  src/export.cpp
  src/import.cpp
  src/new.cpp
  src/search.cpp
  libs/easyloggingpp/src/easylogging++.cc)
target_link_libraries(wk PRIVATE
  fmt::fmt
  SQLiteCpp  
  yaml-cpp
  nlohmann_json::nlohmann_json
  sqlite3 pthread dl)

IF(NOT USE_ALT_STDFS)
target_link_libraries(wk PRIVATE stdc++fs)  # Work around GCC8/GCC9 on Ubuntu
ENDIF(NOT USE_ALT_STDFS)
