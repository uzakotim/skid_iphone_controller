cmake_minimum_required(VERSION 3.6.2)
project(DEPOMME VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(cppzmq REQUIRED)
# Include Eigen
find_package(Eigen3 REQUIRED)

include_directories(include /usr/local/include) 
add_definitions(-DPROJECT_FOLDER_PATH="${CMAKE_SOURCE_DIR}")
add_executable(run_me src/main.cpp include/node/functions.h include/node/libraries.h)
target_link_libraries(run_me PUBLIC Eigen3::Eigen cppzmq)
target_compile_options(run_me PRIVATE -Wno-psabi)

