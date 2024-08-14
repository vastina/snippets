#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = "#version 450 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "out vec3 ourColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n"
                                 "   ourColor = aColor;\n"
                                 "}\0";
const char* fragmentShaderSource = "#version 450 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(ourColor, 1.0f);\n"
                                   "}\n\0";
void when_resize( GLFWwindow* window, int width, int height )
{
  glViewport( width / 20, height / 20, ( width - width / 20 ), ( height - height / 20 ) );
}
void esc_quit( GLFWwindow* window )
{
  if ( glfwGetKey( window, 256 /*escape*/ ) == GLFW_PRESS )
    glfwSetWindowShouldClose( window, true );
}
int main( void )
{
  glfwInit();
  GLFWwindow* w = glfwCreateWindow( 1000, 700, "w", nullptr, nullptr );
  glfwMakeContextCurrent( w );
  glfwSetFramebufferSizeCallback( w, when_resize );
  if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) ) {
    std::cout << "glad" << std::endl;
    system( "read" );
    return -1;
  }
  float vertices[] = {
    -0.5f,
    -0.5f,
    0.0f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
  };
  unsigned vertex_shader = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vertex_shader, 1, &vertexShaderSource, nullptr );
  glCompileShader( vertex_shader );
  unsigned fragement_shader = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fragement_shader, 1, &fragmentShaderSource, nullptr );
  glCompileShader( fragement_shader );

  unsigned shader_program = glCreateProgram();
  glAttachShader( shader_program, vertex_shader );
  glAttachShader( shader_program, fragement_shader );
  glLinkProgram( shader_program );
  glDeleteShader( vertex_shader );
  glDeleteShader( fragement_shader );

  unsigned vbo, vao;
  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );

  while ( !glfwWindowShouldClose( w ) ) {
    esc_quit( w );
    glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shader_program );
    glBindVertexArray( vao );
    glDrawArrays( GL_TRIANGLES, 0, 3 );
    glfwSwapBuffers( w );
    glfwPollEvents();
  }
  glDeleteVertexArrays( 1, &vao );
  glDeleteBuffers( 1, &vbo );
  glDeleteProgram( shader_program );
  glfwTerminate();
  return 0;
}
