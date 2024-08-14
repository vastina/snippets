#include "tool/global.hpp"

#include "tool/camera.hpp"
#include "tool/shader.hpp"
#include "tool/vertex.hpp"

#include <array>
#include <map>

extern std::array<float, 180> cubeVertices;
extern std::array<float, 30> planeVertices;
extern std::array<float, 30> transparentVertices;

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;

  glEnable( GL_DEPTH_TEST );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  string currentDir = std::filesystem::path( __FILE__ ).parent_path().string();
  Shader shader( currentDir + "/shader/vertex.glsl", currentDir + "/shader/frag.glsl" );
  vas::VertexBinder cube { cubeVertices.data(), cubeVertices.size() * sizeof( float ), { 3, 2 } };
  vas::VertexBinder plane {
    planeVertices.data(), planeVertices.size() * sizeof( float ), { 3, 2 } };
  vas::VertexBinder transparent {
    transparentVertices.data(), transparentVertices.size() * sizeof( float ), { 3, 2 } };

  u32 cubeTexture = loadTexture( "./static/pictures/marble.jpg" );
  u32 floorTexture = loadTexture( "./static/pictures/metal.png" );
  u32 transparentTexture = loadTexture( "./static/pictures/window.png" );

  // transparent window locations
  vector<glm::vec3> windows { glm::vec3( -1.5f, 0.0f, -0.48f ),
                              glm::vec3( 1.5f, 0.0f, 0.51f ),
                              glm::vec3( 0.0f, 0.0f, 0.7f ),
                              glm::vec3( -0.3f, 0.0f, -2.3f ),
                              glm::vec3( 0.5f, 0.0f, -0.6f ) };

  shader.use();
  shader.setInt( "texture1", 0 );

  Timer time;
  while ( !glfwWindowShouldClose( w ) ) {
    time.Update();
    camera.processWindowInput( w, time.deltaTime );
    ProcessInput( w );

    std::map<float, glm::vec3> sorted;
    for ( unsigned int i = 0; i < windows.size(); i++ ) {
      float distance = glm::length( camera.Position - windows[i] );
      sorted[distance] = windows[i];
    }

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // draw objects
    shader.use();
    glm::mat4 projection = glm::perspective(
      glm::radians( camera.Zoom ), (float)screenwidth / (float)screenheight, 0.1f, 100.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4( 1.0f );
    shader.setMat4( "projection", projection );
    shader.setMat4( "view", view );
    // cubes

    cube.Bind();
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, cubeTexture );
    model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    model = glm::mat4( 1.0f );
    model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    // floor
    plane.Bind();
    glBindTexture( GL_TEXTURE_2D, floorTexture );
    model = glm::mat4( 1.0f );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    // windows (from furthest to nearest)
    transparent.Bind();
    glBindTexture( GL_TEXTURE_2D, transparentTexture );
    for ( std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend();
          ++it ) {
      model = glm::mat4( 1.0f );
      model = glm::translate( model, it->second );
      shader.setMat4( "model", model );
      glDrawArrays( GL_TRIANGLES, 0, 6 );
    }

    glfwSwapBuffers( w );
    glfwPollEvents();
  }

  cube.Clear();
  plane.Clear();
  transparent.Clear();
  glfwTerminate();
  return 0;
}