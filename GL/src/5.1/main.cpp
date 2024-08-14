#include "tool/global.hpp"

#include "tool/shader.hpp"
#include "tool/vertex.hpp"

#include <array>

extern std::array<float, 180> cubeVertices;
extern std::array<float, 30> planeVertices;

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;

  // configure global opengl state
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_ALWAYS ); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

  Shader shader( "./5.2/shader/vertex.glsl", "./5.2/shader/frag.glsl" );
  vas::VertexBinder cube { cubeVertices.data(), cubeVertices.size() * sizeof( float ), { 3, 2 } };
  vas::VertexBinder plane {
    planeVertices.data(), planeVertices.size() * sizeof( float ), { 3, 2 } };
  u32 cubeTexture = loadTexture( "./static/pictures/marble.jpg" );
  u32 floorTexture = loadTexture( "./static/pictures/metal.png" );

  shader.use();
  shader.setInt( "texture1", 0 );

  Timer time;
  while ( not glfwWindowShouldClose( w ) ) {
    time.Update();
    camera.processWindowInput( w, time.deltaTime );
    ProcessInput( w );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glm::mat4 model = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), 1.0f, 0.1f, 100.0f );

    shader.use();
    shader.setMat4( "view", view );
    shader.setMat4( "projection", projection );

    glActiveTexture( GL_TEXTURE0 );
    // cubes
    cube.Bind();
    glBindTexture( GL_TEXTURE_2D, cubeTexture );
    model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 2.0f, 0.0f, 0.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    // floor
    plane.Bind();
    glBindTexture( GL_TEXTURE_2D, floorTexture );
    shader.setMat4( "model", glm::mat4( 1.0f ) );
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glBindVertexArray( 0 );

    glfwSwapBuffers( w );
    glfwPollEvents();
  }

  cube.Clear();
  plane.Clear();
  glfwTerminate();
}
