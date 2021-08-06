cmake_minimum_required(VERSION 3.0.0)
project(ideal-gas-particle-paths)

include_directories(${CMAKE_SOURCE_DIR}/includes)

find_package(OpenMP REQUIRED)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(ideal-gas-particle-paths ${SOURCES})
target_link_libraries(ideal-gas-particle-paths OpenMP::OpenMP_CXX)
