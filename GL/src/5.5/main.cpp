#include "tool/global.hpp"

#include "tool/shader.hpp"
#include "tool/vertex.hpp"

#include <array>

extern std::array<float, 180> cubeVertices;
extern std::array<float, 30> planeVertices;
extern std::array<float, 6 * 4> quadVertices;

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;

  glEnable( GL_DEPTH_TEST );

  Shader shader( "./5.5/shader/framebuffers.vs", "./5.5/shader/framebuffers.fs" );
  Shader screenShader( "./5.5/shader/framebuffers_screen.vs",
                       "./5.5/shader/framebuffers_screen.fs" );

  vas::VertexBinder cube { cubeVertices.data(), cubeVertices.size() * sizeof( float ), { 3, 2 } };
  vas::VertexBinder plane {
    planeVertices.data(), planeVertices.size() * sizeof( float ), { 3, 2 } };
  vas::VertexBinder quad { quadVertices.data(), quadVertices.size() * sizeof( float ), { 2, 2 } };

  u32 cubeTexture = loadTexture( "./static/pictures/container1.jpg" );
  u32 floorTexture = loadTexture( "./static/pictures/metal.png" );

  shader.use();
  shader.setInt( "texture1", 0 );

  screenShader.use();
  screenShader.setInt( "screenTexture", 0 );

  // framebuffer configuration
  u32 framebuffer;
  glGenFramebuffers( 1, &framebuffer );
  glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
  // create a color attachment texture
  u32 textureColorbuffer;
  glGenTextures( 1, &textureColorbuffer );
  glBindTexture( GL_TEXTURE_2D, textureColorbuffer );
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, screenwidth, screenheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0 );
  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  u32 rbo;
  glGenRenderbuffers( 1, &rbo );
  glBindRenderbuffer( GL_RENDERBUFFER, rbo );
  glRenderbufferStorage(
    GL_RENDERBUFFER,
    GL_DEPTH24_STENCIL8,
    screenwidth,
    screenheight ); // use a single renderbuffer object for both a depth AND stencil buffer.
  // now actually attach it
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo );
  // now that we actually created the framebuffer and added all attachments we want to check if it
  // is actually complete now
  if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << '\n';
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

  Timer timer;
  while ( not glfwWindowShouldClose( w ) ) {
    timer.Update();
    camera.processWindowInput( w, timer.deltaTime );
    ProcessInput( w );

    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
    glEnable( GL_DEPTH_TEST ); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glm::mat4 model = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
      glm::radians( camera.Zoom ), (float)screenwidth / (float)screenheight, 0.1f, 100.0f );

    shader.use();
    shader.setMat4( "view", view );
    shader.setMat4( "projection", projection );

    glActiveTexture( GL_TEXTURE0 );
    cube.Bind();
    glBindTexture( GL_TEXTURE_2D, cubeTexture );
    model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 2.0f, 0.0f, 0.0f ) );
    shader.setMat4( "model", model );
    glDrawArrays( GL_TRIANGLES, 0, 36 );

    plane.Bind();
    glBindTexture( GL_TEXTURE_2D, floorTexture );
    shader.setMat4( "model", glm::mat4( 1.0f ) );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindVertexArray( 0 );

    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer
    // color texture
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDisable(
      GL_DEPTH_TEST ); // disable depth test so screen-space quad isn't discarded due to depth test.
    // clear all relevant buffers
    glClearColor(
      1.0f, 1.0f, 1.0f, 1.0f ); // set clear color to white (not really necessary actually, since we
                                // won't be able to see behind the quad anyways)
    glClear( GL_COLOR_BUFFER_BIT );

    screenShader.use();
    quad.Bind();
    glBindTexture(
      GL_TEXTURE_2D,
      textureColorbuffer ); // use the color attachment texture as the texture of the quad plane
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glfwSwapBuffers( w );
    glfwPollEvents();
  }

  cube.Clear();
  plane.Clear();
  quad.Clear();
  glfwTerminate();
  return 0;
}
