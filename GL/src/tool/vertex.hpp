#ifndef _VERTEXT_H_
#define _VERTEXT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "tool/util.hpp"

namespace vas {

struct VertexBinder
{
  u32 vao {}; // vertex array object
  u32 vbo {}; // vertex buffer object

  VertexBinder( const float* vertices,
                u32 vertexSize,
                const vector<u32>& layout,
                u32 usage = GL_STATIC_DRAW )
  {
    glGenVertexArrays( 1, &vao );
    glGenBuffers( 1, &vbo );
    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vertexSize, vertices, usage );
    u32 stride = 0;
    for ( auto i : layout )
      stride += i;
    u32 offset = 0;
    for ( u32 i = 0; i < layout.size(); ++i ) {
      glEnableVertexAttribArray( i );
      glVertexAttribPointer( i,
                             layout[i],
                             GL_FLOAT,
                             GL_FALSE,
                             stride * sizeof( float ),
                             (char*)0 + offset * sizeof( float ) );
      offset += layout[i];
    }
  }
  void Bind() const { glBindVertexArray( vao ); }
  void Clear()
  {
    glDeleteVertexArrays( 1, &vao );
    glDeleteBuffers( 1, &vbo );
  }

  ~VertexBinder() { /*Clear();*/ }
};

}

#endif