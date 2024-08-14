#ifndef _GL_WINDOW_GUI_CXX_H_
#define _GL_WINDOW_GUI_CXX_H_

#include <glad/glad.h>
#include "tool/global.hpp"
#include "tool/gui.hpp"

#include <functional>

namespace vas {

class Window
{
private:
  GLFWwindow* window;
  vector<std::function<void()>> UiCallbacks;
  string UiTitle { "w" };
  bool UiEnabled { false };
  bool UiClosed { false };

public:
  Window( const char* title = "w", int width = screenwidth, int height = screenheight )
  {
    window = glfwCreateWindow( width, height, title, nullptr, nullptr );
    if ( window == nullptr ) {
      throw std::runtime_error( "Failed to create GLFW window" );
    }
    glfwMakeContextCurrent( window );

    glfwSetFramebufferSizeCallback( window, []( GLFWwindow* /*window*/, int width, int height ) {
      glViewport( 0, 0, width, height );
    } );
    glfwSetCursorPosCallback( window, CursorPosCallback );
    glfwSetScrollCallback( window, ScrollCallback );
    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) ) {
      throw std::runtime_error( "Failed to load GLAD" );
    }
    glfwSwapInterval( 1 );
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    InitImGui();
  }
  ~Window()
  {
    CleanupImGui();
    glfwDestroyWindow( window );
  }

  inline void SubmitUi( const char* description, float& float_value, float v_min, float v_max )
  {
    UiCallbacks.push_back( [description, &float_value, v_min, v_max]() {
      ImGui::SliderFloat( description, &float_value, v_min, v_max );
    } );
  }
  inline void SubmitUi( const char* description, int& int_value, int v_min, int v_max )
  {
    UiCallbacks.push_back( [description, &int_value, v_min, v_max]() {
      ImGui::SliderInt( description, &int_value, v_min, v_max );
    } );
  }
  inline void SubmitUi( const char* description, bool& bool_value )
  {
    UiCallbacks.push_back(
      [description, &bool_value]() { ImGui::Checkbox( description, &bool_value ); } );
  }
  inline void SubmitUi( const char* description )
  {
    UiCallbacks.push_back( [description]() { ImGui::Text( description ); } );
  }
  inline void SubmitUi( const char* description, const char* char_p_value )
  {
    UiCallbacks.push_back(
      [description, char_p_value]() { ImGui::Text( description, char_p_value ); } );
  }
  inline void SubmitUi( const char* description, string& str_value )
  {
    UiCallbacks.push_back(
      [description, &str_value]() { ImGui::Text( description, str_value.data() ); } );
  }
  inline void SubmitUi( const char* description, ImVec4& ImVec4_value, bool Edit_w )
  {
    if ( Edit_w )
      UiCallbacks.push_back( [description, &ImVec4_value]() {
        ImGui::ColorEdit4( description, reinterpret_cast<float*>( &ImVec4_value ) );
      } );
    else
      UiCallbacks.push_back( [description, &ImVec4_value]() {
        ImGui::ColorEdit3( description, reinterpret_cast<float*>( &ImVec4_value ) );
      } );
  }
  inline void SubmitUi( const char* description, ImVec4* ImVec4_value )
  {
    UiCallbacks.push_back( [description, ImVec4_value]() {
      ImGui::ColorEdit3( description, reinterpret_cast<float*>( ImVec4_value ) );
    } );
  }
  void OpenUi() { UiClosed = false; }
  void CloseUi() { UiClosed = true; }
  void ClearUi() { UiCallbacks.clear(); }

  void Process()
  {
    glfwPollEvents();

    if ( !UiClosed ) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      {
        ImGui::Begin( UiTitle.data() );
        for ( const auto& callback : UiCallbacks )
          callback();
        ImGui::End();
      }
      ImGui::Render();
    }

    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
      UiEnabled = !UiEnabled;
      if ( UiEnabled )
        glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
      else
        glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    }
  }
  bool ShouldClose() { return glfwWindowShouldClose( window ); }
  void FreshBuffers()
  {
    if ( !UiClosed ) {
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    }
    glfwSwapBuffers( window );
  }
  void Close() { glfwSetWindowShouldClose( window, true ); }
  void ChangeTitle( const char* title ) { glfwSetWindowTitle( window, title ); }
  void ChangeUiTitle( const char* title ) { UiTitle = string( title ); }
  auto* GetWindow() { return window; }

private:
  void InitImGui() { ::InitImGui( window ); }
};

}

#endif