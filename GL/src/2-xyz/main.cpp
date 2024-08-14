#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "global.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb/stb_image.h>
#include "tool/shader.hpp"
void print( const glm::mat4& m ) {}
void when_resize( GLFWwindow* window, int width, int height )
{
  glViewport( 0, 0, width, height );
}
void processInput( GLFWwindow* window )
{
  if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
    glfwSetWindowShouldClose( window, true );
  float cameraSpeed = static_cast<float>( 2.5 * deltaTime );
}
const int screenwidth = 800;
const int screenheight = 800;
double lastX = (double)screenwidth / 2.0;
double lastY = (double)screenheight / 2.0;
double sensitivity = 0.013;
float speed = 0.1;
glm::vec3 ws = glm::vec3( 0.0, 0.0, -1.0f );
glm::vec3 ad = glm::vec3( -1.0f, 0.0, 0.0 );
float yaw = -90.0f, dyaw = 0.0;
float pitch = 0.0f, dpitch = 0.0;
glm::vec3 camerapos = glm::vec3( 0.0, 0.0, -4.0f );
void mouse_callback( GLFWwindow* window, double xpos, double ypos )
{
  dyaw = sensitivity * ( xpos - lastX );
  dpitch = sensitivity * ( ypos - lastY );
  yaw += dyaw;
  pitch += dpitch;
  ad = glm::vec3( cos( glm::radians( yaw + 90.0f ) ), 0.0, sin( glm::radians( yaw + 90.0f ) ) );
  if ( pitch > 89.0 )
    pitch = 89.0;
  if ( pitch < -89.0 )
    pitch = -89.0;
  ws = glm::vec3( cos( glm::radians( pitch ) ) * cos( glm::radians( yaw ) ),
                  -sin( glm::radians( pitch ) ),
                  cos( glm::radians( pitch ) ) * sin( glm::radians( yaw ) ) );
  lastX = xpos;
  lastY = ypos;
}
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset ) {}
int main()
{
  glfwInit();
  GLFWwindow* w = glfwCreateWindow( 800, 800, "w", nullptr, nullptr );
  glfwMakeContextCurrent( w );
  glfwSetFramebufferSizeCallback( w, when_resize );
  glfwSetCursorPosCallback( w, mouse_callback );
  glfwSetScrollCallback( w, scroll_callback );
  if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
    return -1;
  glfwSetInputMode( w, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
  Shader shader( "./2-xyz/shader/vertex.glsl", "./2-xyz/shader/frag.glsl" );
  u32 vbo, vao /*, ebo*/;
  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void*)0 );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer(
    1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void*)( 3 * sizeof( float ) ) );
  glEnableVertexAttribArray( 1 );
  stbi_set_flip_vertically_on_load( true );
  int width, height, nrchannels;
  u32 texture1, texture2;
  glGenTextures( 1, &texture1 );
  glBindTexture( GL_TEXTURE_2D, texture1 );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  unsigned char* data = stbi_load( "./static/pictures/3.jpg", &width, &height, &nrchannels, 0 );
  if ( data ) {
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );
  } else {
    std::cout << "fail" << std::endl;
    return 1;
  }
  glGenTextures( 1, &texture2 );
  glBindTexture( GL_TEXTURE_2D, texture2 );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  unsigned char* data2 = stbi_load( "./static/pictures/a.png", &width, &height, &nrchannels, 0 );
  if ( data2 ) {
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2 );
    glGenerateMipmap( GL_TEXTURE_2D );
  } else {
    std::cout << "fail" << std::endl;
    return 1;
  }
  stbi_image_free( data );
  stbi_image_free( data2 );
  shader.use();
  shader.setInt( "texture1", 0 );
  shader.setInt( "texture2", 1 );
  glEnable( GL_DEPTH_TEST );
  while ( !glfwWindowShouldClose( w ) ) {
    glClearColor( 0.30f, 0.15f, 0.65f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    processInput( w );
    float currentFrame = static_cast<float>( glfwGetTime() );
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, texture2 );
    shader.use();
    float t = glfwGetTime(), temp = ( t - pi / 2.0 ) * 1.0;
    glm::mat4 transform =
      /*glm::transpose(glm::mat4(
              0.5*cos(t)+0.7f, 0.0f, 0.0f, 0.0f,
              0.0f, 0.5 * cos(t) + 0.7f, 0.0f, 0.0f,
              0.0f, 0.0f, 0.5 * cos(t) + 0.7f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f)) */
      glm::transpose( glm::mat4( 1.0f,
                                 0.0f,
                                 0.0f,
                                 0.3 * cos( t ),
                                 0.0f,
                                 1.0f,
                                 0.0f,
                                 0.3 * sin( t ),
                                 0.0f,
                                 0.0f,
                                 1.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 1.0f ) )
      * glm::transpose( glm::mat4( cos( temp ),
                                   -sin( temp ),
                                   0.0f,
                                   0.0f,
                                   sin( temp ),
                                   cos( temp ),
                                   0.0f,
                                   0.0f,
                                   0.0f,
                                   0.0f,
                                   1.0f,
                                   0.0f,
                                   0.0f,
                                   0.0f,
                                   0.0f,
                                   1.0f ) );
    shader.setMat4( "trans1", transform );
    glm::mat4 view = glm::mat4( 1.0f );
    view = glm::rotate( view, glm::radians( yaw + 90.0f ), glm::vec3( 0.0, 1.0, 0.0 ) );
    view = glm::rotate( view, glm::radians( pitch ), ad );
    if ( glfwGetKey( w, GLFW_KEY_W ) == GLFW_PRESS ) {
      camerapos -= ws * speed;
    }
    if ( glfwGetKey( w, GLFW_KEY_S ) == GLFW_PRESS ) {
      camerapos += ws * speed;
    }
    if ( glfwGetKey( w, GLFW_KEY_A ) == GLFW_PRESS ) {
      camerapos += ad * speed;
    }
    if ( glfwGetKey( w, GLFW_KEY_D ) == GLFW_PRESS ) {
      camerapos -= ad * speed;
    }
    view = glm::translate( view, camerapos );
    shader.setMat4( "view", view );
    glm::mat4 projection = glm::mat4( 1.0f );
    projection = glm::perspective( (float)glm::radians( 45.0f ), 1.0f, 0.1f, 100.0f );
    shader.setMat4( "projection", projection );
    glBindVertexArray( vao );
    for ( u32 i = 0; i < 10; i++ ) {
      glm::mat4 model = glm::mat4( 1.0f );
      model = glm::translate( model, cubePositions[i] );
      float angle = 20.0f * (float)i;
      model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 1.5f, 0.0f ) );
      shader.setMat4( "model", model );
      glDrawArrays( GL_TRIANGLES, 0, 36 );
    }
    glfwSwapBuffers( w );
    glfwPollEvents();
  }
  glDeleteVertexArrays( 1, &vao );
  glDeleteBuffers( 1, &vbo );
  glfwTerminate();
  return 0;
}