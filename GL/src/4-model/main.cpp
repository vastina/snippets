#include "tool/global.hpp"

#include "tool/model.hpp"
#include "tool/vertex.hpp"

#include <format>
#include "vertices.hpp.in"

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;

  Shader modelshader( "./4-model/shader/nanosuitvertex.glsl",
                      "./4-model/shader/nanosuitfrag.glsl" );
  Model nanosuit( "./static/model/nanosuit/nanosuit.obj" );
  std::vector<int> model_vt {};
  modelshader.use();
  for ( const auto& tex : nanosuit.textures_loaded ) {
    model_vt.push_back( tex.id );
    modelshader.setInt( tex.type, static_cast<int>( model_vt.size() - 1 ) );
  }

  // there is an error with rock1, so I use rock2.obj instead
  Shader rockshader( "./4-model/shader/rock_vertex.glsl", "./4-model/shader/rock_frag.glsl" );
  Model rock( "./static/model/rock2/rock.obj" );
  std::vector<u32> rock_vt {};
  rockshader.use();
  for ( auto& tex : rock.textures_loaded ) {
    rock_vt.push_back( tex.id );
    rockshader.setInt( tex.type, static_cast<int>( rock_vt.size() - 1 ) );
  }

  Shader shader( "./4-model/shader/vertex.glsl", "./4-model/shader/frag.glsl" );
  vas::VertexBinder v1 { vertices, sizeof( vertices ), { 3, 2, 3, 3 } };
  Shader lightshader( "./4-model/shader/light_vertex.glsl",
                      "./4-model/shader/light_fragcolor.glsl" );
  vas::VertexBinder lightv1 { lightvertices, sizeof( lightvertices ), { 3 } };

  u32 diffusemap = loadTexture( "./static/pictures/container2.png" );
  u32 specularmap = loadTexture( "./static/pictures/container2_specular.png" );
  u32 emissionmap = loadTexture( "./static/pictures/awesomeface.png" );
  glm::vec3 sunambient = glm::vec3( 1.0f, 0.5f, 0.2f ) * glm::vec3( 0.1f );
  glm::vec3 sundiffuse = glm::vec3( 1.0f, 0.5f, 0.2f ) * glm::vec3( 0.4f );
  glm::vec3 sunspecular = glm::vec3( 1.0f, 1.0f, 1.0f );

  shader.use();
  shader.setInt( "material.diffuse", 0 );
  shader.setInt( "material.specular", 1 );
  shader.setInt( "material.emission", 2 );

  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LESS );

  Timer time;
  while ( !glfwWindowShouldClose( w ) ) {
    glClearColor( 0.2f, 0.2f, 0.3f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    time.Update();
    camera.processWindowInput( w, time.deltaTime );
    ProcessInput( w );

    float t = static_cast<float>( glfwGetTime() );
    // constexpr float pi = glm::pi<float>();
    // float temp = ( t - pi / 2.f ) * 2.5f;
    glm::mat4 transform = glm::mat4( 1.0f );
    /* glm::transpose(glm::mat4(
            0.5*cos(t * 2.0)+0.7f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5 * cos(t * 2.0) + 0.7f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5 * cos(t * 2.0) + 0.7f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f)) *
    glm::transpose(glm::mat4(
            1.0f, 0.0f, 0.0f, 1.5 * cos(t),
            0.0f, 1.0f, 0.0f, 2.5 * sin(t),
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f)) *
    glm::transpose(glm::mat4(
            cos(temp),-sin(temp), 0.0f, 0.0f,
            sin(temp), cos(temp), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f));*/
    glm::mat4 model = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection
      = glm::perspective( (float)glm::radians( camera.Zoom ), 1.0f, 0.1f, 50.0f );
    glm::vec3 lightColor( 1.0, 1.0, 1.0 );
    lightColor.r = sin( t * 2.0f );
    lightColor.g = sin( t * 0.7f );
    lightColor.b = sin( t * 1.3f );
    glm::vec3 _lightpos[4];

    modelshader.use();
    {
      for ( int i = 0; i < model_vt.size(); i++ ) {
        glActiveTexture( GL_TEXTURE0 + i );
        glBindTexture( GL_TEXTURE_2D, model_vt[i] );
      }
      for ( u32 i = 0; i < 4; i++ ) {
        modelshader.setVec3( std::format( "light[{}].position", i ), _lightpos[i] );
        modelshader.setVec3( std::format( "light[{}].ambient", i ),
                             point_color[i] * glm::vec3( 0.1f ) );
        modelshader.setVec3( std::format( "light[{}].diffuse", i ),
                             point_color[i] * glm::vec3( 0.4f ) );
        modelshader.setVec3( std::format( "light[{}].specular", i ), point_color[i] );
      }
      modelshader.setFloat( "material.shininess", 128.0 );
      modelshader.setFloat( "lightstrength.constant", 1.0 );
      modelshader.setFloat( "lightstrength.linear", 0.09f );
      modelshader.setFloat( "lightstrength.quadratic", 0.032f );

      modelshader.setVec3( "sunlight.direction", -0.2f, -1.0f, -0.3f );
      modelshader.setVec3( "sunlight.ambient", sunambient );
      modelshader.setVec3( "sunlight.diffuse", sundiffuse );
      modelshader.setVec3( "sunlight.specular", sunspecular );

      modelshader.setVec3( "flashlight.direction", camera.Front );
      modelshader.setVec3( "flashlight.position", camera.Position );
      modelshader.setFloat( "flashlight.cutoff", glm::cos( glm::radians( 12.5f ) ) );
      modelshader.setFloat( "flashlight.outer_cutoff", glm::cos( glm::radians( 17.5f ) ) );
      modelshader.setVec3( "flashlight.ambient", 0.1f, 0.1f, 0.1f );
      modelshader.setVec3( "flashlight.diffuse", 0.8f, 0.8f, 0.8f );
      modelshader.setVec3( "flashlight.specular", lightColor );

      modelshader.setVec3( "viewpos", camera.Position );
      model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.f, 0.f, 4.f ) );
      model = glm::scale( model, glm::vec3( 0.2f ) );
      modelshader.setMat4( "model", model );
      modelshader.setMat4( "view", view );
      modelshader.setMat4( "projection", projection );
    }
    nanosuit.Draw( modelshader );

    rockshader.use();
    {
      for ( u32 i = 0; i < rock_vt.size(); i++ ) {
        glActiveTexture( GL_TEXTURE0 + i );
        glBindTexture( GL_TEXTURE_2D, rock_vt[i] );
      }
      for ( u32 i = 0; i < 4; i++ ) {
        rockshader.setVec3( std::format( "light[{}].position", i ), _lightpos[i] );
        rockshader.setVec3( std::format( "light[{}].ambient", i ),
                            point_color[i] * glm::vec3( 0.1f ) );
        rockshader.setVec3( std::format( "light[{}].diffuse", i ),
                            point_color[i] * glm::vec3( 0.4f ) );
        rockshader.setVec3( std::format( "light[{}].specular", i ), point_color[i] );
      }
      rockshader.setFloat( "material.shininess", 16.0 );
      rockshader.setFloat( "lightstrength.constant", 1.0 );
      rockshader.setFloat( "lightstrength.linear", 0.09f );
      rockshader.setFloat( "lightstrength.quadratic", 0.032f );

      rockshader.setVec3( "sunlight.direction", -0.2f, -1.0f, -0.3f );
      rockshader.setVec3( "sunlight.ambient", sunambient );
      rockshader.setVec3( "sunlight.diffuse", sundiffuse );
      rockshader.setVec3( "sunlight.specular", sunspecular );

      rockshader.setVec3( "flashlight.direction", camera.Front );
      rockshader.setVec3( "flashlight.position", camera.Position );
      rockshader.setFloat( "flashlight.cutoff", glm::cos( glm::radians( 12.5f ) ) );
      rockshader.setFloat( "flashlight.outer_cutoff", glm::cos( glm::radians( 17.5f ) ) );
      rockshader.setVec3( "flashlight.ambient", 0.1f, 0.1f, 0.1f );
      rockshader.setVec3( "flashlight.diffuse", 0.8f, 0.8f, 0.8f );
      rockshader.setVec3( "flashlight.specular", lightColor );

      rockshader.setVec3( "viewpos", camera.Position );
      model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0, -5.0, 0.0 ) );
      model = glm::scale( model, glm::vec3( 10.f, 3.f, 10.f ) );
      rockshader.setMat4( "model", model );
      rockshader.setMat4( "view", view );
      rockshader.setMat4( "projection", projection );
      rockshader.setVec3( "pos_offset", glm::vec3( 0.0, 0.0, 0.0 ) );
    }
    rock.Draw( rockshader );
    {
      model = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0, 10.0, 0.0 ) );
      model = glm::scale( model, glm::vec3( 10.f, 2.f, 10.f ) );
      rockshader.setMat4( "model", model );
      rockshader.setVec3( "pos_offset", glm::vec3( 1.0f, 0.0f, 1.0f ) );
    }
    rock.Draw( rockshader );

    lightshader.use();
    {
      lightshader.setMat4( "view", view );
      lightshader.setMat4( "projection", projection );
      lightv1.Bind();
      for ( u32 i = 0; i < 4; i++ ) {
        glm::mat4 lightmodel = glm::translate( glm::mat4( 1.0f ), pointLightPositions[i] );
        lightmodel = glm::scale( lightmodel, glm::vec3( 0.006f ) );
        lightshader.setMat4( "model", lightmodel );
        _lightpos[i] = glm::vec3( lightmodel * glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
        lightshader.setVec3( "_light_fragcolor", point_color[i] );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
      }
    }

    shader.use();
    {
      shader.setMat4( "trans1", transform );
      shader.setMat4( "view", view );
      shader.setMat4( "projection", projection );
      for ( u32 i = 0; i < 4; i++ ) {
        shader.setVec3( std::format( "light[{}].position", i ), _lightpos[i] );
        shader.setVec3( std::format( "light[{}].ambient", i ), point_color[i] * glm::vec3( 0.1f ) );
        shader.setVec3( std::format( "light[{}].diffuse", i ), point_color[i] * glm::vec3( 0.4f ) );
        shader.setVec3( std::format( "light[{}].specular", i ), point_color[i] );
      }

      shader.setFloat( "lightstrength.constant", 1.0 );
      shader.setFloat( "lightstrength.linear", 0.09f );
      shader.setFloat( "lightstrength.quadratic", 0.032f );

      shader.setVec3( "sunlight.direction", -0.2f, -1.0f, -0.3f );
      shader.setVec3( "sunlight.ambient", sunambient );
      shader.setVec3( "sunlight.diffuse", sundiffuse );
      shader.setVec3( "sunlight.specular", sunspecular );

      shader.setVec3( "flashlight.direction", camera.Front );
      shader.setVec3( "flashlight.position", camera.Position );
      shader.setFloat( "flashlight.cutoff", glm::cos( glm::radians( 12.5f ) ) );
      shader.setFloat( "flashlight.outer_cutoff", glm::cos( glm::radians( 17.5f ) ) );
      shader.setVec3( "flashlight.ambient", 0.1f, 0.1f, 0.1f );
      shader.setVec3( "flashlight.diffuse", 0.8f, 0.8f, 0.8f );
      shader.setVec3( "flashlight.specular", lightColor );

      shader.setVec3( "viewpos", camera.Position );
      shader.setFloat( "material.shininess", 32.0 );
      shader.setFloat( "move", t );
      shader.setFloat( "t", t );

      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, diffusemap );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, specularmap );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, emissionmap );

      v1.Bind();
      for ( u32 i = 0; i < 10; i++ ) {
        model = glm::translate( glm::mat4( 1.0f ), cubePositions[i] );
        float angle = 20.0f * (float)i;
        model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 1.5f, 0.0f ) );
        shader.setMat4( "model", model );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
      }
    }

    glfwSwapBuffers( w );
    glfwPollEvents();
  }

  v1.Clear();
  lightv1.Clear();
  glfwTerminate();
  return 0;
}
