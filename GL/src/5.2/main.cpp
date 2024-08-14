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

  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LESS );
  glEnable( GL_STENCIL_TEST );
  glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
  glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

  Shader shader( "./5.2/shader/vertex.glsl", "./5.2/shader/frag.glsl" );
  Shader shadersinglecolor( "./5.2/shader/vertex.glsl", "./5.2/shader/singlecolorfrag.glsl" );

  vas::VertexBinder cube {
    cubeVertices.data(), cubeVertices.size() * sizeof( float ), { 3, 2 }, GL_STATIC_DRAW };
  vas::VertexBinder plane {
    planeVertices.data(), planeVertices.size() * sizeof( float ), { 3, 2 } };

  u32 cubeTexture = loadTexture( "./static/pictures/container1.jpg" );
  u32 floorTexture = loadTexture( "./static/pictures/metal.png" );

  shader.use();
  shader.setInt( "texture1", 0 );

  Timer timer;
  while ( not glfwWindowShouldClose( w ) ) {
    timer.Update();
    camera.processWindowInput( w, timer.deltaTime );
    ProcessInput( w );

    // make sure we clear the framebuffer's content
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glm::mat4 model = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
      glm::radians( camera.Zoom ), (float)screenwidth / (float)screenheight, 0.1f, 100.0f );

    shadersinglecolor.use();
    shadersinglecolor.setMat4( "view", view );
    shadersinglecolor.setMat4( "projection", projection );

    shader.use();
    shader.setMat4( "view", view );
    shader.setMat4( "projection", projection );

    // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the
    // containers. We set its mask to 0x00 to not write to the stencil buffer.
    glStencilMask( 0x00 );
    plane.Bind();
    glBindTexture( GL_TEXTURE_2D, floorTexture );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindVertexArray( 0 );

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    glStencilFunc( GL_ALWAYS, 1, 0xFF );
    glStencilMask( 0xFF );
    cube.Bind();
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, cubeTexture );
    model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 2.0f, 0.0f, 0.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling
    // stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1
    // are not drawn, thus only drawing the objects' size differences, making it look like borders.
    glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
    glStencilMask( 0x00 );
    glDisable( GL_DEPTH_TEST );
    shadersinglecolor.use();
    constexpr float scale = 1.05f;

    cube.Bind();
    glBindTexture( GL_TEXTURE_2D, cubeTexture );
    model = glm::translate( glm::mat4( 1.f ), glm::vec3( -1.0f, 0.0f, -1.0f ) );
    model = glm::scale( model, glm::vec3( scale ) );
    shadersinglecolor.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );

    model = glm::translate( glm::mat4( 1.f ), glm::vec3( 2.0f, 0.0f, 0.0f ) );
    model = glm::scale( model, glm::vec3( scale ) );
    shadersinglecolor.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );

    glBindVertexArray( 0 );
    glStencilMask( 0xFF );
    glStencilFunc( GL_ALWAYS, 0, 0xFF );
    glEnable( GL_DEPTH_TEST );

    glfwSwapBuffers( w );
    glfwPollEvents();
  }

  cube.Clear();
  plane.Clear();
  glfwTerminate();
  return 0;
}
