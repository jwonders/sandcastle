project(property_tree_demo)
cmake_minimum_required(VERSION 2.6)

find_package(Boost 1.46 REQUIRED)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
include_directories("include")
add_executable(propertyTree ${SOURCES})