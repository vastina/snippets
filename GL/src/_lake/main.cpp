#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "tool/util.hpp"
// #include "tool/gui.hpp"

static void checkCompileErrors( u32 shader, const std::string& type )
{
  int success;
  char infoLog[1024];
  if ( type != "program" ) {
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
    if ( !success ) {
      glGetShaderInfoLog( shader, 1024, nullptr, infoLog );
      std::cout << "shader compilation error, type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
    } // shader compilation error, type:
  } else {
    glGetProgramiv( shader, GL_LINK_STATUS, &success );
    if ( !success ) {
      glGetProgramInfoLog( shader, 1024, nullptr, infoLog );
      std::cout << "shader linking error, type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
    }
  }
}

int screenwidth = 1280;
int screenheight = 720;
void framebuffer_size_callback( GLFWwindow* /*window*/, int width, int height )
{
  screenwidth = width;
  screenheight = height;
  glViewport( 0, 0, width, height );
}

float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

constexpr float moveMentSpeed = 2.f;
glm::vec3 ws = glm::vec3( 0.0, 0.0, -1.0f );
glm::vec3 ad = glm::vec3( -1.0f, 0.0f, 0.0f );
glm::vec3 cameraPos = glm::vec3( 1000.f, 20.f, 2500.f );

float yaw = -90.0f;
float pitch = 0.f;
void mouse_callback( GLFWwindow* /*window*/, double xpos, double ypos )
{
  if ( firstMouse ) {
    lastMouseX = xpos;
    lastMouseY = ypos;
    firstMouse = false;
    return;
  }

  constexpr double sensitivity = 0.05;

  double dyaw = sensitivity * ( xpos - lastMouseX );
  double dpitch = sensitivity * ( ypos - lastMouseY );
  yaw += dyaw;
  pitch += dpitch;
  if ( pitch > 89.0 )
    pitch = 89.0;
  if ( pitch < -89.0 )
    pitch = -89.0;
  ad = glm::vec3( cos( glm::radians( yaw + 90.0f ) ), 0.0, sin( glm::radians( yaw + 90.0f ) ) );
  ws = glm::vec3( cos( glm::radians( pitch ) ) * cos( glm::radians( yaw ) ),
                  -sin( glm::radians( pitch ) ),
                  cos( glm::radians( pitch ) ) * sin( glm::radians( yaw ) ) );

  lastMouseX = xpos;
  lastMouseY = ypos;
}

int main()
{
  if ( !glfwInit() ) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  GLFWwindow* window = glfwCreateWindow(
    screenwidth, screenheight, "lake sunrise (press esc to switch watch mode)", nullptr, nullptr );
  if ( !window ) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent( window );
  glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
  glfwSetCursorPosCallback( window, mouse_callback );
  glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );

  // 加载GLAD
  if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) ) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  std::string vertexCode { Readfile( "./src/_lake/shader/vertex.glsl" ) };

  // https://www.shadertoy.com/view/tcS3WD
  std::string fragmentCode { Readfile( "./src/_lake/shader/src.glsl"/*"./src/_lake/shader/frag.glsl"*/ ) };
  const char* vertexShaderSource = vertexCode.data();
  const char* fragmentShaderSource = fragmentCode.data();

  unsigned int vertexShader = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vertexShader, 1, &vertexShaderSource, nullptr );
  glCompileShader( vertexShader );
  checkCompileErrors( vertexShader, "VERTEX" );

  unsigned int fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fragmentShader, 1, &fragmentShaderSource, nullptr );
  glCompileShader( fragmentShader );
  checkCompileErrors( fragmentShader, "FRAGMENT" );

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader( shaderProgram, vertexShader );
  glAttachShader( shaderProgram, fragmentShader );
  glLinkProgram( shaderProgram );

  glDeleteShader( vertexShader );
  glDeleteShader( fragmentShader );

  float vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

  unsigned VAO;
  unsigned VBO;
  glGenVertexArrays( 1, &VAO );
  glGenBuffers( 1, &VBO );

  glBindVertexArray( VAO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ), (void*)0 );
  glEnableVertexAttribArray( 0 );

  bool watch_mode = false; // default watch mode : video

  u32 frame_count = 0u;
  auto begin { std::chrono::high_resolution_clock::now() };
  while ( !glfwWindowShouldClose( window ) ) {
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
      static u32 last_count = 0u;
      if ( frame_count - last_count > 5u ) {
        watch_mode = !watch_mode;
        last_count = frame_count;
      }
    }
    if ( watch_mode ) {
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    } else {
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
      // cameraPosForglsl = glm::vec3( 0.f, 20.f, 2500.f );
    }

    if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ) {
      cameraPos += ws * moveMentSpeed;
    }
    if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ) {
      cameraPos -= ws * moveMentSpeed;
    }
    if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ) {
      cameraPos -= ad * moveMentSpeed;
    }
    if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ) {
      cameraPos += ad * moveMentSpeed;
    }

    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( shaderProgram );

    glUniform3fv(
      glGetUniformLocation( shaderProgram, "cameraPos" ), 1, glm::value_ptr( cameraPos ) );
    glUniform3fv( glGetUniformLocation( shaderProgram, "forward" ), 1, glm::value_ptr( ws ) );
    glUniform1f( glGetUniformLocation( shaderProgram, "pitch" ), glm::radians( pitch ) );
    glUniform2f( glGetUniformLocation( shaderProgram, "iResolution" ), screenwidth, screenheight );
    glUniform1f( glGetUniformLocation( shaderProgram, "iTime" ), glfwGetTime() );
    // float aspect = (float)width / (float)height;
    // glUniform1f( glGetUniformLocation( shaderProgram, "uAspect" ), aspect );
    glUniform4f( glGetUniformLocation( shaderProgram, "iMouse" ),
                 lastMouseX,
                 lastMouseY,
                 watch_mode ? 0.01f : 0.f,
                 0 );

    glBindVertexArray( VAO );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glfwSwapBuffers( window );
    glfwPollEvents();

    {
      frame_count++;
      using namespace std::chrono_literals;
      auto now { std::chrono::high_resolution_clock::now() };
      if ( now - begin >= 1s ) {
        // std::cout << frame_count << ' ';
        frame_count = 0u;
        begin = now;
      }
    }
  }

  glDeleteVertexArrays( 1, &VAO );
  glDeleteBuffers( 1, &VBO );
  glDeleteProgram( shaderProgram );

  glfwTerminate();
  return 0;
}