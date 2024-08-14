#include "tool/window.hpp"
#include "tool/shader.hpp"
#include "tool/model.hpp"

int main()
{
  try {
    InitEnv();
    vas::Window w( "5.9" );
    w.CloseUi();

    glEnable( GL_DEPTH_TEST );

    Shader shader(
      "./5.9/shader/vertex.glsl", "./5.9/shader/frag.glsl", "./5.9/shader/geometry.glsl" );
    Model nanosuit( "./static/model/nanosuit/nanosuit.obj" );
    Model rock( "./static/model/rock2/rock.obj" );

    Timer timer;
    while ( not w.ShouldClose() ) {
      timer.Update();
      camera.processWindowInput( w.GetWindow(), timer.deltaTime );
      w.Process();

      glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      glm::mat4 projection = glm::perspective(
        glm::radians( camera.Zoom ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f );
      glm::mat4 view = camera.GetViewMatrix();
      glm::mat4 model = glm::mat4( 1.0f );

      shader.use();
      shader.setMat4( "projection", projection );
      shader.setMat4( "view", view );
      shader.setMat4( "model", model );
      shader.setFloat( "time", timer.lastFrame );

      nanosuit.Draw( shader );

      model = glm::translate( model, glm::vec3( 0.0f, -6.0f, 0.0f ) );
      model = glm::scale( model, glm::vec3( 3.f ) );
      shader.setMat4( "model", model );
      rock.Draw( shader );

      w.FreshBuffers();
    }
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  CleanupEnv();
  return 0;
}