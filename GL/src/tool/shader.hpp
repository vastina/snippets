#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

#include "tool/util.hpp"

// a temp solution for file path
static inline string FileLocation( const string& path )
{
  if ( path.find( "src" ) != string::npos )
    return path;
  return string( "./src/" ) + path;
}

class Shader
{
  u32 ID;
public:
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader( const std::string& vertexPath,
          const std::string& fragmentPath,
          const char* geometryPath = nullptr )
  {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode { Readfile( FileLocation( vertexPath ) ) };
    std::string fragmentCode { Readfile( FileLocation( fragmentPath ) ) };

    const char* vShaderCode = vertexCode.data();
    const char* fShaderCode = fragmentCode.data();
    // 2. compile shaders
    unsigned vertex;
    unsigned fragment;
    u32 geometry;
    // vertex shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    checkCompileErrors( vertex, "vertex" );
    // fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    checkCompileErrors( fragment, "fragment" );
    // shader Program
    ID = glCreateProgram();
    glAttachShader( ID, vertex );
    glAttachShader( ID, fragment );
    if ( geometryPath != nullptr ) {
      string geometryCode { Readfile( FileLocation( geometryPath ) ) };
      const char* gShaderCode = geometryCode.data();
      geometry = glCreateShader( GL_GEOMETRY_SHADER );
      glShaderSource( geometry, 1, &gShaderCode, nullptr );
      glCompileShader( geometry );
      checkCompileErrors( geometry, "geometry" );
      glAttachShader( ID, geometry );
    }
    glLinkProgram( ID );
    checkCompileErrors( ID, "program" );
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    if ( geometryPath != nullptr )
      glDeleteShader( geometry );
  }
  // activate the shader
  // ------------------------------------------------------------------------
  void use() const { glUseProgram( ID ); }
  // utility uniform functions
  // ------------------------------------------------------------------------
  void setBool( const std::string& name, bool value ) const
  {
    glUniform1i( glGetUniformLocation( ID, name.data() ), (int)value );
  }
  // ------------------------------------------------------------------------
  void setInt( const std::string& name, int value ) const
  {
    glUniform1i( glGetUniformLocation( ID, name.data() ), value );
  }
  // ------------------------------------------------------------------------
  void setFloat( const std::string& name, float value ) const
  {
    glUniform1f( glGetUniformLocation( ID, name.data() ), value );
  }
  void setVec2( const std::string& name, const glm::vec2& value ) const
  {
    glUniform2fv( glGetUniformLocation( ID, name.data() ), 1, &value[0] );
  }
  void setVec2( const std::string& name, float x, float y ) const
  {
    glUniform2f( glGetUniformLocation( ID, name.data() ), x, y );
  }
  // ------------------------------------------------------------------------
  void setVec3( const std::string& name, const glm::vec3& value ) const
  {
    glUniform3fv( glGetUniformLocation( ID, name.data() ), 1, &value[0] );
  }
  void setVec3( const std::string& name, float x, float y, float z ) const
  {
    glUniform3f( glGetUniformLocation( ID, name.data() ), x, y, z );
  }
  // ------------------------------------------------------------------------
  void setVec4( const std::string& name, const glm::vec4& value ) const
  {
    glUniform4fv( glGetUniformLocation( ID, name.data() ), 1, &value[0] );
  }
  void setVec4( const std::string& name, float x, float y, float z, float w ) const
  {
    glUniform4f( glGetUniformLocation( ID, name.data() ), x, y, z, w );
  }
  // ------------------------------------------------------------------------
  void setMat2( const std::string& name, const glm::mat2& mat ) const
  {
    glUniformMatrix2fv( glGetUniformLocation( ID, name.data() ), 1, GL_FALSE, &mat[0][0] );
  }
  // ------------------------------------------------------------------------
  void setMat3( const std::string& name, const glm::mat3& mat ) const
  {
    glUniformMatrix3fv( glGetUniformLocation( ID, name.data() ), 1, GL_FALSE, &mat[0][0] );
  }
  // ------------------------------------------------------------------------
  void setMat4( const std::string& name, const glm::mat4& mat ) const
  {
    glUniformMatrix4fv( glGetUniformLocation( ID, name.data() ), 1, GL_FALSE, &mat[0][0] );
  }

private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  static void checkCompileErrors( u32 shader, const std::string& type )
  {
    int success;
    char infoLog[1024];
    if ( type != "program" ) {
      glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
      if ( !success ) {
        glGetShaderInfoLog( shader, 1024, nullptr, infoLog );
        std::cout << "shader compilation error, type: " << type << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- "
                  << std::endl;
      } // shader compilation error, type:
    } else {
      glGetProgramiv( shader, GL_LINK_STATUS, &success );
      if ( !success ) {
        glGetProgramInfoLog( shader, 1024, nullptr, infoLog );
        std::cout << "shader linking error, type: " << type << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- "
                  << std::endl;
      }
    }
  }
};

struct Material
{
  glm::vec3 Ambient;
  glm::vec3 Diffuse;
  glm::vec3 Specular;
  float Shininess;
  Material( glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess )
    : Ambient( ambient ), Diffuse( diffuse ), Specular( specular ), Shininess( shininess ) {};
  float ambient()
  {
    return ( 0.212671 * Ambient[0] + 0.715160 * Ambient[1] + 0.072169 * Ambient[2] )
           / ( 0.212671 * Diffuse[0] + 0.715160 * Diffuse[1] + 0.072169 * Diffuse[2] );
  }
};
// Material emerald( glm::vec3( 0.0215, 0.1745, 0.0215 ),
//                   glm::vec3( 0.07568, 0.61424, 0.07568 ),
//                   glm::vec3( 0.633, 0.727811, 0.633 ),
//                   0.6 );
// Material jade( glm::vec3( 0.135, 0.2225, 0.1575 ),
//                glm::vec3( 0.54, 0.89, 0.63 ),
//                glm::vec3( 0.316228, 0.316228, 0.316228 ),
//                0.1 );
// Material obsidian( glm::vec3( 0.05375, 0.05, 0.06625 ),
//                    glm::vec3( 0.18275, 0.17, 0.22525 ),
//                    glm::vec3( 0.332741, 0.328634, 0.346435 ),
//                    0.3 );
// Material pearl( glm::vec3( 0.25, 0.20725, 0.20725 ),
//                 glm::vec3( 1, 0.829, 0.829 ),
//                 glm::vec3( 0.296648, 0.296648, 0.296648 ),
//                 0.088 );
// Material ruby( glm::vec3( 0.1745, 0.01175, 0.01175 ),
//                glm::vec3( 0.61424, 0.04136, 0.04136 ),
//                glm::vec3( 0.727811, 0.626959, 0.626959 ),
//                0.6 );
// Material turquoise( glm::vec3( 0.1, 0.18725, 0.1745 ),
//                     glm::vec3( 0.396, 0.74151, 0.69102 ),
//                     glm::vec3( 0.297254, 0.30829, 0.306678 ),
//                     0.1 );
// Material brass( glm::vec3( 0.32941, 0.22353, 0.02745 ),
//                 glm::vec3( 0.78039, 0.56863, 0.11373 ),
//                 glm::vec3( 0.992157, 0.941176, 0.807843 ),
//                 0.21794872 );
// Material bronze( glm::vec3( 0.2125, 0.1275, 0.054 ),
//                  glm::vec3( 0.714, 0.4284, 0.18144 ),
//                  glm::vec3( 0.393548, 0.271906, 0.166721 ),
//                  0.2 );
// Material chrome( glm::vec3( 0.25, 0.25, 0.25 ),
//                  glm::vec3( 0.4, 0.4, 0.4 ),
//                  glm::vec3( 0.77458, 0.774597, 0.774597 ),
//                  0.6 );
// Material copper( glm::vec3( 0.19125, 0.0735, 0.0225 ),
//                  glm::vec3( 0.7038, 0.27048, 0.0828 ),
//                  glm::vec3( 0.256777, 0.137622, 0.086014 ),
//                  0.1 );
// Material gold( glm::vec3( 0.24725, 0.1995, 0.0745 ),
//                glm::vec3( 0.75164, 0.60648, 0.22648 ),
//                glm::vec3( 0.628281, 0.555802, 0.366065 ),
//                0.4 );
// Material silver( glm::vec3( 0.19225, 0.19225, 0.19225 ),
//                  glm::vec3( 0.50754, 0.50754, 0.50754 ),
//                  glm::vec3( 0.508273, 0.508273, 0.508273 ),
//                  0.4 );
// Material black_plastic( glm::vec3( 0.0, 0.0, 0.0 ),
//                         glm::vec3( 0.01, 0.01, 0.01 ),
//                         glm::vec3( 0.50, 0.50, 0.50 ),
//                         0.25 );
// Material green_plastic( glm::vec3( 0.0, 0.0, 0.0 ),
//                         glm::vec3( 0.1, 0.35, 0.1 ),
//                         glm::vec3( 0.45, 0.55, 0.45 ),
//                         0.25 );
// Material red_plastic( glm::vec3( 0.0, 0.0, 0.0 ),
//                       glm::vec3( 0.5, 0.0, 0.0 ),
//                       glm::vec3( 0.7, 0.6, 0.6 ),
//                       0.25 );
// Material white_plastic( glm::vec3( 0.0, 0.0, 0.0 ),
//                         glm::vec3( 0.55, 0.55, 0.55 ),
//                         glm::vec3( 0.70, 0.70, 0.70 ),
//                         0.25 );
// Material yellow_plastic( glm::vec3( 0.0, 0.0, 0.0 ),
//                          glm::vec3( 0.5, 0.5, 0.0 ),
//                          glm::vec3( 0.60, 0.60, 0.50 ),
//                          0.25 );
// Material black_rubber( glm::vec3( 0.02, 0.02, 0.02 ),
//                        glm::vec3( 0.01, 0.01, 0.01 ),
//                        glm::vec3( 0.4, 0.4, 0.4 ),
//                        0.078125 );
// Material cyan_rubber( glm::vec3( 0.0, 0.05, 0.05 ),
//                       glm::vec3( 0.4, 0.5, 0.5 ),
//                       glm::vec3( 0.04, 0.7, 0.7 ),
//                       0.078125 );
// Material green_rubber( glm::vec3( 0.0, 0.05, 0.0 ),
//                        glm::vec3( 0.4, 0.5, 0.4 ),
//                        glm::vec3( 0.04, 0.7, 0.04 ),
//                        0.078125 );
// Material red_rubber( glm::vec3( 0.05, 0.0, 0.0 ),
//                      glm::vec3( 0.5, 0.4, 0.4 ),
//                      glm::vec3( 0.7, 0.04, 0.04 ),
//                      0.078125 );
// Material white_rubber( glm::vec3( 0.05, 0.05, 0.05 ),
//                        glm::vec3( 0.5, 0.5, 0.5 ),
//                        glm::vec3( 0.7, 0.7, 0.7 ),
//                        0.078125 );
// Material yellow_rubber( glm::vec3( 0.05, 0.05, 0.0 ),
//                         glm::vec3( 0.5, 0.5, 0.4 ),
//                         glm::vec3( 0.7, 0.7, 0.04 ),
//                         0.078125 );
// Material cyan_plastic( glm::vec3( 0.0, 0.1, 0.06 ),
//                        glm::vec3( 0.0, 0.50980392, 0.50980392 ),
//                        glm::vec3( 0.50196078, 0.50196078, 0.50196078 ),
//                        0.25 );

#endif