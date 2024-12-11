cmake_minimum_required (VERSION 3.10)

project(hh)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -fsanitize=address -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

file(GLOB src "./*.cpp")
add_executable(hhh ${src})
target_include_directories(hhh PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(hhh PRIVATE ${OpenCV_LIBS})