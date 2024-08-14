#version 450 core
layout( location = 0 ) in vec3 _Pos;
layout( location = 1 ) in vec2 _TexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  TexCoords = _TexCoords;
  gl_Position = projection * view * model * vec4( _Pos, 1.0 );
}