cmake_minimum_required(VERSION 3.8)

project(GL LANGUAGES CXX)

if(WIN32)
  # Enable Hot Reload for MSVC compilers if supported. conflict with address
  # sanitizer if ( POLICY CMP0141 ) cmake_policy( SET CMP0141 NEW )
  # set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
  # "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
  # ) endif()
  set(env "C:/Users/71958/AppData/Local/.xmake/packages") # packages location
  include_directories(
    "D:\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.40.33807\\include"
  ) # clangd cat't find it automatically
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /W3 -fsanitize=address /Zi")
elseif(LINUX)
  set(env "/home/vastina/.xmake/packages") # packages location
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -Og -fsanitize=address -g")
endif()
cmake_policy(SET CMP0074 NEW) # for xxx_root
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(glad_ROOT "${env}/g/glad/v0.1.36/b918abab79c04749ba8b220ab08bfbb8")
  set(glfw3_ROOT "${env}/g/glfw/3.4/fc47358da159466996f7d289b4cf1b4e")
  set(stb_include_dir
      "${env}/s/stb/2024.06.01/50b5205a30f145adb70e146a5d0967dc/include")
  set(glm_include_dir
      "${env}/g/glm/1.0.1/788496219b2d40629f92bac6907b6bba/include")
  set(assimp_ROOT "${env}/a/assimp/v5.4.2/40d1ec5cb1e74852bfd2210304407fce")
  # set(imgui_ROOT "${env}/i/imgui/v1.90.9/bd309073bda64e7184726431b0bceffc")
  # find_package(imgui REQUIRED) set(IMGUI_LIBRARIES imgui::imgui)
  # set(Vulkan_ROOT "D:/VulkanSDK/1.3.290.0")
elseif(LINUX)
  set(glad_ROOT "${env}/g/glad/v0.1.36/f31cd39a27f14bdc8dd8413392e1ee53")
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(IMGUI REQUIRED imgui)
endif()
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
# find_package(Vulkan REQUIRED)

# for the fucking imgui
if(WIN32)
  set(IMGUI_DIR "${env}/i/imgui-1.91.0")
  set(IMGUI_FILES
      ${IMGUI_DIR}/imgui.cpp
      ${IMGUI_DIR}/imgui_draw.cpp
      ${IMGUI_DIR}/imgui_widgets.cpp
      ${IMGUI_DIR}/imgui_tables.cpp
      ${IMGUI_DIR}/imgui_demo.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)
  add_library(imgui_lib STATIC ${IMGUI_FILES})
  target_include_directories(imgui_lib PRIVATE ${IMGUI_DIR})
  target_link_libraries(imgui_lib PRIVATE glfw)
  set(IMGUI_INCLUDE_DIRS "${IMGUI_DIR}/include.user") # for .gitignore contains
                                                      # *.user
  set(IMGUI_LIBRARIES imgui_lib)
endif()

function(setup_project dir_name)
  set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
  set(${dir_name}_INCLUDE "${SOURCE_DIR}/${dir_name}/include")
  file(GLOB SOURCES_${dir_name} "${SOURCE_DIR}/${dir_name}/*.cpp")
  if(WIN32)
    set(_outname "${dir_name}")
  elseif(LINUX)
    set(_outname "${dir_name}.out")
  endif()
  add_executable(${_outname} ${SOURCES_${dir_name}})
  if(WIN32)
    set_property(TARGET ${_outname} PROPERTY CXX_STANDARD 20
                                             CXX_STANDARD_REQUIRED YES)
    set_target_properties(${_outname} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} /utf-8")
    # set_target_properties(
    #   ${_outname}
    #   PROPERTIES WIN32_EXECUTABLE TRUE
    #              LINK_FLAGS "${LINK_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()
  target_include_directories(
    ${_outname}
    PRIVATE ${SOURCE_DIR}
            ${glm_include_dir}
            ${stb_include_dir}
            ${GLAD_INCLUDE_DIRS}
            ${ASSIMP_INCLUDE_DIRS}
            ${IMGUI_INCLUDE_DIRS}
            ${IMGUI_INCLUDE_DIRS}/imgui)
  target_link_libraries(
    ${_outname} PRIVATE glfw glad::glad ${ASSIMP_LIBRARIES} ${IMGUI_LIBRARIES}
                        # Vulkan::Vulkan
  )
endfunction()

setup_project(0-imgui-test)
setup_project(1-hello)
setup_project(2-xyz) # Coordinate system
setup_project(3-light1)
setup_project(3-light2)
setup_project(4-model)
setup_project(5.1) # depth test
setup_project(5.2) # stencil test
setup_project(5.3) # blending sort
setup_project(5.5) # framebuffer
setup_project(5.9) # geometry shader exploding
setup_project(5.10)# asteroids
setup_project(6-light)

setup_project(_lake)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
