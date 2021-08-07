cmake_minimum_required(VERSION 3.0.0)
project(ideal-gas-particle-paths)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/includes ${CMAKE_SOURCE_DIR}/vendors/olc_pixel_game_engine)

find_package(OpenMP REQUIRED)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)

find_package(X11)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(ideal-gas-particle-paths ${SOURCES})
target_link_libraries(ideal-gas-particle-paths OpenMP::OpenMP_CXX ${PNG_LIBRARY} OpenGL::GL ${X11_LIBRARIES})
