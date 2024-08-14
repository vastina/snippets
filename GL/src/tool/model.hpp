#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "tool/mesh.hpp"

#include <iostream>
#include <vector>

static inline u32 TextureFromFile( const char* path,
                                   const string& directory,
                                   bool /*gamma*/ = false )
{
  string filename = string( path );
  filename = directory + '/' + filename;

  return loadTexture( filename.data() );
}
class Model
{
public:
  vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make
                                   // sure textures aren't loaded more than once.
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  Model( const string& path, bool gamma = false ) : gammaCorrection( gamma ) { loadModel( path ); }

  void Draw( Shader& shader )
  {
    for ( const auto& mesh : meshes )
      mesh.Draw( shader );
  }

private:
  void loadModel( const string& path )
  {
    // read file via ASSIMP
    Assimp::Importer importer {};
    const aiScene* scene = importer.ReadFile( path,
                                              aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );
    // check for errors
    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
         || !scene->mRootNode ) // if is Not Zero
    {
      std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << '\n';
      return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr( 0, path.find_last_of( '/' ) );

    // process ASSIMP's root node recursively
    processNode( scene->mRootNode, scene );
  }

  // processes a node in a recursive fashion. Processes each individual mesh located at the node and
  // repeats this process on its children nodes (if any).
  void processNode( aiNode* node, const aiScene* scene )
  {
    // process each mesh located at the current node
    for ( u32 i = 0; i < node->mNumMeshes; i++ ) {
      // the node object only contains indices to index the actual objects in the scene.
      // the scene contains all the data, node is just to keep stuff organized (like relations
      // between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back( processMesh( mesh, scene ) );
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the
    // children nodes
    for ( u32 i = 0; i < node->mNumChildren; i++ ) {
      processNode( node->mChildren[i], scene );
    }
  }
  Mesh processMesh( aiMesh* mesh, const aiScene* scene )
  {
    // data to fill
    vector<Vertex> vertices;
    vector<u32> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for ( u32 i = 0; i < mesh->mNumVertices; i++ ) {
      Vertex vertex;
      // we declare a placeholder vector since assimp uses its own vector class
      // that doesn't directly convert to glm's vec3 class so we transfer the data
      // to this placeholder glm::vec3 first. positions
      vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
      // normals
      if ( mesh->HasNormals() ) {
        vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
      }
      // texture coordinates
      if ( mesh->mTextureCoords[0] ) // does the mesh contain texture coordinates?
      {
        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption
        // that we won't use models where a vertex can have multiple texture coordinates so we
        // always take the first set (0).
        vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        // tangent
        vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        // bitangent
        vertex.Bitangent
          = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
      } else
        vertex.TexCoords = glm::vec2( 0.0f, 0.0f );

      vertices.push_back( vertex );
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the
    // corresponding vertex indices.
    for ( u32 i = 0; i < mesh->mNumFaces; i++ ) {
      u32 len = mesh->mFaces[i].mNumIndices;
      for ( u32 j = 0; j < len; j++ )
        indices.push_back( mesh->mFaces[i].mIndices[j] );
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps
      = loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );
    textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );
    // 2. specular maps
    vector<Texture> specularMaps
      = loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
    textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );
    // 3. normal maps
    std::vector<Texture> normalMaps
      = loadMaterialTextures( material, aiTextureType_HEIGHT, "texture_normal" );
    textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
    // 4. height maps
    std::vector<Texture> heightMaps
      = loadMaterialTextures( material, aiTextureType_AMBIENT, "texture_height" );
    textures.insert( textures.end(), heightMaps.begin(), heightMaps.end() );

    // return a mesh object created from the extracted mesh data
    return Mesh( vertices, indices, textures );
  }

  vector<Texture> loadMaterialTextures( aiMaterial* mat,
                                        aiTextureType type,
                                        const string& typeName )
  {
    vector<Texture> textures;
    for ( u32 i = 0; i < mat->GetTextureCount( type ); i++ ) {
      aiString str;
      mat->GetTexture( type, i, &str );
      // check if texture was loaded before and if so, continue to next iteration: skip loading a
      // new texture
      bool skip = false;
      for ( u32 j = 0; j < textures_loaded.size(); j++ ) {
        if ( std::strcmp( textures_loaded[j].path.data(), str.C_Str() ) == 0 ) {
          textures.push_back( textures_loaded[j] );
          skip = true; // a texture with the same filepath has already been loaded, continue to next
                       // one. (optimization)
          break;
        }
      }
      if ( !skip ) { // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = TextureFromFile( str.C_Str(), this->directory );
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back( texture );
        textures_loaded.push_back( texture ); // store it as texture loaded for entire model, to
                                              // ensure we won't unnecesery load duplicate textures.
      }
    }
    return textures;
  }
};

#endif