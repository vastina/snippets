set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)


add_executable(basic_test basic/basic.cpp)
target_link_libraries(basic_test ${project_name})
