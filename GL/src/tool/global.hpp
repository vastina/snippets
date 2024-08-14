// #if defined (__gl_h_) || defined (__glad_h_) || defined (GLM_VERSION_MAJOR) || defined (max) ||
// defined (_glfw3_h_)
//   #error "This file must include at top of main.cpp"
// #endif
// a helper header
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "tool/camera.hpp"
#include "tool/util.hpp"

#include <iostream>

constexpr int screenwidth = 800;
constexpr int screenheight = 800;
constexpr u32 SCR_WIDTH = screenwidth;
constexpr u32 SCR_HEIGHT = screenheight;

static Camera camera( glm::vec3( 0.0f, 0.0f, 4.0f ) );
static inline void ProcessInput( GLFWwindow* window )
{
  if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
    glfwSetWindowShouldClose( window, true );
}
static inline void FramebufferSizeCallback( GLFWwindow* /*window*/, int width, int height )
{
  glViewport( 0, 0, width, height );
}

static inline void CursorPosCallback( GLFWwindow* /*window*/, double xposIn, double yposIn )
{
  static bool firstMouse = true;
  static float lastX = screenwidth / 2.f;
  static float lastY = screenheight / 2.f;

  float xpos = static_cast<float>( xposIn );
  float ypos = static_cast<float>( yposIn );
  if ( firstMouse ) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement( xoffset, yoffset );
}
static inline void ScrollCallback( GLFWwindow* /*window*/, double /*xoffset*/, double yoffset )
{
  camera.ProcessMouseScroll( static_cast<float>( yoffset ) );
}

static inline GLFWwindow* InitWindow( const char* title = "w",
                                      int width = screenwidth,
                                      int height = screenheight )
{
  static bool initialized = false;
  if ( initialized )
    return nullptr;
  glfwInit();
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
  GLFWwindow* w = glfwCreateWindow( width, height, title, nullptr, nullptr );
  if ( w == nullptr ) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent( w );
  glfwSetFramebufferSizeCallback( w, FramebufferSizeCallback );
  glfwSetCursorPosCallback( w, CursorPosCallback );
  glfwSetScrollCallback( w, ScrollCallback );
  if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
    return nullptr;
  glfwSwapInterval( 1 );
  glfwSetInputMode( w, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

  stbi_set_flip_vertically_on_load( true );

  initialized = true;
  return w;
}

static inline void InitEnv()
{
  static bool initialized = false;
  if ( initialized )
    return;
  if ( not glfwInit() ) {
    throw std::runtime_error( "Failed to initialize GLFW" );
  }
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
  // glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  stbi_set_flip_vertically_on_load( true );

  initialized = true;
}

static inline void CleanupEnv()
{
  glfwTerminate();
}