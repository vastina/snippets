#include "tool/global.hpp"
#include "tool/shader.hpp"
#include "tool/vertex.hpp"

#include <array>

extern std::array<float, 11 * 6 * 6> vertices;
extern std::array<float, 3 * 6 * 6> lightvertices;
extern std::array<glm::vec3, 10> cubePositions;

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;

  Shader shader( "./3-light1/shader/vertex.glsl", "./3-light1/shader/frag.glsl" );
  vas::VertexBinder v1 { vertices.data(), vertices.size() * sizeof( float ), { 3, 2, 3, 3 } };
  Shader lightshader( "./3-light1/shader/light_vertex.glsl",
                      "./3-light1/shader/light_fragcolor.glsl" );
  vas::VertexBinder lightv1 { lightvertices.data(), lightvertices.size() * sizeof( float ), { 3 } };

  glEnable( GL_DEPTH_TEST );

  Material white_rubber( glm::vec3( 0.05, 0.05, 0.05 ),
                         glm::vec3( 0.5, 0.5, 0.5 ),
                         glm::vec3( 0.7, 0.7, 0.7 ),
                         0.078125 );
  Timer time;
  while ( !glfwWindowShouldClose( w ) ) {
    glClearColor( 0.05f, 0.05f, 0.05f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    time.Update();
    camera.processWindowInput( w, time.deltaTime );
    ProcessInput( w );

    glm::mat4 transform = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective( (float)glm::radians( 45.0f ), 1.0f, 0.1f, 100.0f );

    glm::vec3 lightPos( 0.0f, 0.0f, -5.0f );
    glm::mat4 lightmodel
      = glm::scale( glm::translate( glm::mat4( 1.0f ), lightPos ), glm::vec3( 0.1f ) );
    glm::vec3 lightColor( 1.0, 1.0, 1.0 );
    glm::vec3 _lightPos = glm::vec3( lightmodel * glm::vec4( lightPos, 1.0 ) );
    glm::vec3 diffuseColor = lightColor * glm::vec3( 0.5f );
    glm::vec3 ambientColor = diffuseColor * glm::vec3( 0.2f );

    lightshader.use();
    lightshader.setMat4( "model", lightmodel );
    lightshader.setMat4( "view", view );
    lightshader.setMat4( "projection", projection );
    lightv1.Bind();
    glDrawArrays( GL_TRIANGLES, 0, 36 );

    shader.use();
    shader.setMat4( "trans1", transform );
    // glUniformMatrix4fv( glGetUniformLocation( shader.ID, "view" ), 1, GL_FALSE, &view[0][0] );
    shader.setMat4( "view", view );
    shader.setMat4( "projection", projection );
    shader.setVec3( "light.position", _lightPos );
    shader.setVec3( "light.ambient", ambientColor );
    shader.setVec3( "light.diffuse", diffuseColor );
    shader.setVec3( "light.specular", 1.0f, 1.0f, 1.0f );
    shader.setVec3( "material.ambient", white_rubber.Ambient );
    shader.setVec3( "material.diffuse", white_rubber.Diffuse );
    shader.setVec3( "material.specular", white_rubber.Specular );
    shader.setFloat( "material.shininess", white_rubber.Shininess * 128.0 );
    shader.setVec3( "viewpos", camera.Position );
    v1.Bind();
    for ( u32 i = 0; i < 10; i++ ) {
      glm::mat4 model = glm::translate( glm::mat4( 1.0f ), cubePositions[i] );
      float angle = 20.0f * (float)i;
      model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 1.5f, 0.0f ) );
      shader.setMat4( "model", model );
      glDrawArrays( GL_TRIANGLES, 0, 36 );
    }

    glfwSwapBuffers( w );
    glfwPollEvents();
  }
  v1.Clear();
  lightv1.Clear();
  glfwTerminate();
  return 0;
}