#include "tool/window.hpp"
#include "tool/shader.hpp"
#include "tool/model.hpp"

int main()
{
  try {
    InitEnv();
    camera = Camera( glm::vec3( 0.0f, 0.0f, 44.0f ) );
    vas::Window w( "5.10" );

    Shader shader( "./5.10/shader/vertex.glsl", "./5.10/shader/frag.glsl" );
    Model rock( "./static/model/rock2/rock.obj" );
    Model planet( "./static/model/planet/planet.obj" );

    constexpr u32 Count { 1000u };
    vector<glm::mat4> modelMatrices( Count );

    srand( time( nullptr ) );
    vector<float> rotAngles( Count );
    for ( auto& angle : rotAngles )
      angle = static_cast<float>( ( rand() % 360 ) );
    vector<float> Scales( Count );
    for ( auto& scale : Scales )
      scale = static_cast<float>( ( rand() % 20 ) / 100.0 + 0.05 );

    const auto genModelMatrix { [&rotAngles, &Scales, &modelMatrices]( const float angle_offst ) {
      constexpr auto genDisplacements { []() {
        constexpr float offset = 2.5f;
        srand( time( nullptr ) );
        vector<float> Displacements( Count );
        for ( auto& direction : Displacements ) {
          direction = ( rand() % (int)( 2 * offset * 100 ) ) / 100.0f - offset;
        }
        return Displacements;
      } };
      const static auto displacementXs { genDisplacements() };
      const static auto displacementYs { genDisplacements() };
      const static auto displacementZs { genDisplacements() };
      constexpr float radius = 50.f;
      for ( int i = 0; auto& modelMatrix : modelMatrices ) {
        glm::mat4 model = glm::mat4( 1.0f );
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)modelMatrices.size() * 360.0f;
        float x = sin( angle + angle_offst ) * radius + displacementXs[i];
        float y = displacementYs[i] * 0.4f;
        float z = cos( angle + angle_offst ) * radius + displacementZs[i];
        model = glm::translate( model, glm::vec3( x, y, z ) );
        // 2. scale: Scale between 0.05 and 0.25f
        model = glm::scale( model, glm::vec3( Scales[i] ) );
        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        modelMatrix = glm::rotate( model, rotAngles[i], glm::vec3( 0.4f, 0.6f, 0.8f ) );
        i++;
      }
    } };

    Timer timer;

    w.SubmitUi( "hello world" );
    ImVec4 clear_color { 0.1f, 0.1f, 0.1f, 1.0f };
    w.SubmitUi( "clear color", &clear_color );
    w.SubmitUi( "MoveSpeed", camera.MovementSpeed, 1.f, 30.f );

    glEnable( GL_DEPTH_TEST );
    while ( not w.ShouldClose() ) {
      timer.Update();
      camera.processWindowInput( w.GetWindow(), timer.deltaTime );
      w.Process();

      glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      glm::mat4 view = camera.GetViewMatrix();
      glm::mat4 projection = glm::perspective(
        glm::radians( camera.Zoom ), (float)screenwidth / (float)screenheight, 0.1f, 100.0f );
      glm::mat4 model = glm::translate( glm::mat4( 1.f ), glm::vec3( 0.f, -3.f, 0.f ) );
      model = glm::scale( model, glm::vec3( 4.f ) );

      shader.use();
      shader.setMat4( "view", view );
      shader.setMat4( "projection", projection );
      shader.setMat4( "model", model );

      planet.Draw( shader );
      genModelMatrix( timer.lastFrame / 12.f );
      for ( auto& modelMatrix : modelMatrices ) {
        shader.setMat4( "model", modelMatrix );
        rock.Draw( shader );
      }

      w.FreshBuffers();
    }
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  CleanupEnv();
  return 0;
}