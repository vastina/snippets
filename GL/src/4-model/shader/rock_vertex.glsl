#version 450 core
layout( location = 0 ) in vec3 _pos;
layout( location = 1 ) in vec3 _normal;
layout( location = 2 ) in vec2 _texture_coord;
out vec3 normal;
out vec2 texture_coord;
out vec3 fragpos;
uniform vec3 pos_offset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
  normal = mat3( transpose( inverse( model ) ) ) * _normal;
  texture_coord = _texture_coord;
  gl_Position = projection * view * model * vec4( _pos + pos_offset, 1.0 );
}