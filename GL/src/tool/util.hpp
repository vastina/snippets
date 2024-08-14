#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::string;
using std::vector;
using u32 = std::uint32_t;

#ifdef _WIN64
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
// #pragma warning( disable : 4819 )
#undef min
#undef max
#undef RGB
#endif // _WIN64

static inline string Readfile( const string& path )
{
  if ( !std::filesystem::exists( path ) )
    throw std::runtime_error( "File not found: " + path );
  const auto length { std::filesystem::file_size( path ) };
  std::ifstream file { path, std::ios::binary };
  string content( length, 0 );
  file.read( content.data(), length );
  file.close();
  return content;
}

static inline u32 loadTexture( char const* path )
{
  u32 textureID;
  glGenTextures( 1, &textureID );

  int width;
  int height;
  int nrComponents;
  unsigned char* data = stbi_load( path, &width, &height, &nrComponents, 0 );
  if ( data ) {
    GLenum format {};
    if ( nrComponents == 1 )
      format = GL_RED;
    else if ( nrComponents == 3 )
      format = GL_RGB;
    else if ( nrComponents == 4 )
      format = GL_RGBA;

    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    stbi_image_free( data );
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }

  return textureID;
}

struct Timer
{
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  void Update()
  {
    float currentFrame = static_cast<float>( glfwGetTime() );
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
  }
};

#endif