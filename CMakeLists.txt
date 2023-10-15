cmake_minimum_required(VERSION 3.22.1)
project(mongo_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third-party)

add_executable(mongo_cpp main.cpp
               src/mongo-cpp.hpp src/string_view.hpp src/bson-cpp.hpp)
target_link_libraries(mongo_cpp mongocxx_static bsoncxx_static)