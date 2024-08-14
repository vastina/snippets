#ifndef _MESH_H_
#define _MESH_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tool/shader.hpp"
#include "tool/util.hpp"

struct Vertex
{
  glm::vec3 Position;  // 顶点属性
  glm::vec3 Normal;    // 法线
  glm::vec2 TexCoords; // 纹理坐标

  // 切线空间属性
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
};

struct Texture
{
  u32 id;
  string type;
  string path;
};

class Mesh
{
public:
  // mesh Data
  vector<Vertex> vertices;
  vector<u32> indices;
  vector<Texture> textures;
  u32 VAO;

  Mesh( const vector<Vertex>& Vertices,
        const vector<u32>& Indices,
        const vector<Texture>& Textures )
    : vertices( Vertices ), indices( Indices ), textures( Textures )
  {
    setupMesh();
  }
  // render the mesh
  void Draw( Shader& shader ) const
  {
    // bind appropriate textures
    u32 diffuseNr = 1;
    u32 specularNr = 1;
    u32 normalNr = 1;
    u32 heightNr = 1;
    for ( u32 i = 0; i < textures.size(); i++ ) {
      glActiveTexture( GL_TEXTURE0 + i ); // active proper texture unit before binding
                                          // retrieve texture number (the N in diffuse_textureN)
      string number;
      string name = textures[i].type;
      if ( name == "texture_diffuse" )
        number = std::to_string( diffuseNr++ );
      else if ( name == "texture_specular" )
        number = std::to_string( specularNr++ ); // transfer u32 to stream
      else if ( name == "texture_normal" )
        number = std::to_string( normalNr++ ); // transfer u32 to stream
      else if ( name == "texture_height" )
        number = std::to_string( heightNr++ ); // transfer u32 to stream

      // now set the sampler to the correct texture unit
      shader.setInt( name + number, i );
      // glUniform1i( glGetUniformLocation( shader.ID, ( name + number ).data() ), i );
      // and finally bind the texture
      glBindTexture( GL_TEXTURE_2D, textures[i].id );
    }

    // draw mesh
    glBindVertexArray( VAO );
    glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );

    // always good practice to set everything back to defaults once configured.
    glActiveTexture( GL_TEXTURE0 );
  }

private:
  // render data
  u32 VBO, EBO;

  void setupMesh()
  {
    // create buffers/arrays
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );

    glBindVertexArray( VAO );
    // load data into vertex buffers
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to
    // a glm::vec3/2 array which again translates to 3/2 floats which translates to a byte array.
    glBufferData(
      GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), vertices.data(), GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( u32 ), indices.data(), GL_STATIC_DRAW );

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( Vertex ),
                           (char*)( nullptr ) + ( offsetof( Vertex, Position ) ) );
    // vertex normals
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( Vertex ),
                           (char*)( nullptr ) + ( offsetof( Vertex, Normal ) ) );
    // vertex texture coords
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( Vertex ),
                           (char*)( nullptr ) + offsetof( Vertex, TexCoords ) );
    // vertex tangent
    glEnableVertexAttribArray( 3 );
    glVertexAttribPointer( 3,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( Vertex ),
                           (char*)( nullptr ) + offsetof( Vertex, Tangent ) );
    // vertex bitangent
    glEnableVertexAttribArray( 4 );
    glVertexAttribPointer( 4,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( Vertex ),
                           (char*)( nullptr ) + offsetof( Vertex, Bitangent ) );

    glBindVertexArray( 0 );
  }
};

#endif