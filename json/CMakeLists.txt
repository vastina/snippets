cmake_minimum_required(VERSION 3.9)

set(project_name json++)
project(${project_name})

add_compile_options("-Wall" "-Wextra")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(project_dir "./code")
include_directories("${project_dir}/include")

file(GLOB SOURCES_BASE "${project_dir}/json/base/*.cpp")
file(GLOB SOURCES "${project_dir}/json/*.cpp")
add_library(${project_name} STATIC ${SOURCES_BASE} ${SOURCES})

add_executable(main.out ${project_dir}/main.cpp)
target_link_libraries(main.out json++)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/)

add_subdirectory(${CMAKE_SOURCE_DIR}/testcase)
