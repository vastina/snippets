#include "tool/global.hpp"
#include "tool/shader.hpp"
#include "tool/vertex.hpp"
#include "tool/gui.hpp"

#include <format>
#include <array>

extern std::array<float, 11 * 6 * 6> vertices;
extern std::array<float, 3 * 6 * 6> lightvertices;
extern std::array<glm::vec3, 10> cubePositions;
extern std::array<glm::vec3, 4> pointLightPositions;

static inline void ProcessWindowInput( GLFWwindow* window )
{
  static bool enableUi { false };
  enableUi = !enableUi;
  if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
    if ( enableUi )
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    else
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
  }
}

int main()
{
  GLFWwindow* w = InitWindow();
  if ( w == nullptr )
    return -1;
  InitImGui( w );

  Shader shader( "./3-light2/shader/vertex.glsl", "./3-light2/shader/frag.glsl" );
  vas::VertexBinder v1 { vertices.data(), vertices.size() * sizeof( float ), { 3, 2, 3, 3 } };
  Shader lightshader( "./3-light2/shader/light_vertex.glsl",
                      "./3-light2/shader/light_fragcolor.glsl" );
  vas::VertexBinder lightv1 { lightvertices.data(), lightvertices.size() * sizeof( float ), { 3 } };

  u32 diffusemap = loadTexture( "./static/pictures/container2.png" );
  u32 specularmap = loadTexture( "./static/pictures/container2_specular.png" );
  u32 emissionmap = loadTexture( "./static/pictures/awesomeface.png" );
  shader.use();
  shader.setInt( "material.diffuse", 0 );
  shader.setInt( "material.specular", 1 );
  shader.setInt( "material.emission", 2 );

  glEnable( GL_DEPTH_TEST );

  Timer time;
  bool show_demo_window = false;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
  while ( !glfwWindowShouldClose( w ) ) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      if ( show_demo_window )
        ImGui::ShowDemoWindow( &show_demo_window );

      static float f = 0.0f;
      static int counter = 0;
      ImGui::Begin( "Hello, world!" ); // Create a window called "Hello, world!" and append into it.
      ImGui::Text(
        "This is some useful text." ); // Display some text (you can use a format strings too)
      ImGui::Checkbox( "Demo Window",
                       &show_demo_window ); // Edit bools storing our window open/close state
      ImGui::Checkbox( "Another Window", &show_another_window );
      ImGui::SliderFloat(
        "float", &f, 0.0f, 1.0f ); // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3( "clear color",
                         (float*)&clear_color ); // Edit 3 floats representing a color
      if ( ImGui::Button( "Button" ) ) // Buttons return true when clicked (most widgets return true
                                       // when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text( "counter = %d", counter );
      ImGui::End();

      if ( show_another_window ) {
        ImGui::Begin(
          "Another Window",
          &show_another_window ); // Pass a pointer to our bool variable (the window will have a
                                  // closing button that will clear the bool when clicked)
        ImGui::Text( "Hello from another window!" );
        if ( ImGui::Button( "Close Me" ) )
          show_another_window = false;
        ImGui::End();
      }
    }
    ImGui::Render();

    glClearColor( clear_color.x * clear_color.w,
                  clear_color.y * clear_color.w,
                  clear_color.z * clear_color.w,
                  clear_color.w );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    time.Update();
    camera.processWindowInput( w, time.deltaTime );
    ProcessWindowInput( w );

    float t = glfwGetTime();
    // float temp = ( t - pi / 2.0 ) * 2.5;
    glm::mat4 transform = glm::mat4( 1.0f );
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection
      = glm::perspective( (float)glm::radians( camera.Zoom ), 1.0f, 0.1f, 100.0f );
    glm::vec3 _lightpos[4];
    glm::vec3 lightColor( 1.0, 1.0, 1.0 );
    glm::vec3 diffuseColor = lightColor * glm::vec3( 0.4f );
    glm::vec3 ambientColor = diffuseColor * glm::vec3( 0.1f );

    shader.use();
    {
      shader.setMat4( "trans1", transform );
      shader.setMat4( "view", view );
      shader.setMat4( "projection", projection );
      for ( u32 i = 0; i < 4; i++ ) {
        shader.setVec3( std::format( "light[{}].position", i ), _lightpos[i] );
        shader.setVec3( std::format( "light[{}].ambient", i ), ambientColor );
        shader.setVec3( std::format( "light[{}].diffuse", i ), diffuseColor );
        shader.setVec3( std::format( "light[{}].specular", i ), lightColor );
      }
      shader.setFloat( "lightstrength.constant", 1.0 );
      shader.setFloat( "lightstrength.linear", 0.09f );
      shader.setFloat( "lightstrength.quadratic", 0.032f );
      shader.setVec3( "sunlight.direction", -0.2f, -1.0f, -0.3f );
      shader.setVec3( "sunlight.ambient", ambientColor );
      shader.setVec3( "sunlight.diffuse", diffuseColor );
      shader.setVec3( "sunlight.specular", lightColor );
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
      glm::mat4 model = glm::mat4( 1.0f );
      shader.setMat4( "model", model );
      for ( u32 i = 0; i < 10; i++ ) {
        model = glm::mat4( 1.0f );
        model = glm::translate( model, cubePositions[i] );
        float angle = 20.0f * (float)i;
        model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 1.5f, 0.0f ) );
        shader.setMat4( "model", model );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
      }
    }

    lightshader.use();
    {
      lightshader.setMat4( "view", view );
      lightshader.setMat4( "projection", projection );
      lightv1.Bind();
      for ( u32 i = 0; i < 4; i++ ) {
        glm::mat4 lightmodel = glm::translate( glm::mat4( 1.0f ), pointLightPositions[i] );
        lightmodel = glm::scale( lightmodel, glm::vec3( 0.1f ) );
        lightshader.setMat4( "model", lightmodel );
        _lightpos[i] = glm::vec3( lightmodel * glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
      }
    }

    glfwSwapBuffers( w );
  }

  v1.Clear();
  lightv1.Clear();
  CleanupImGui();
  glfwDestroyWindow( w );
  glfwTerminate();
  return 0;
}
