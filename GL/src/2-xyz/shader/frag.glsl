#version 450 core
out vec4 frag_color;
in vec2 texture_coord;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
  frag_color = mix( texture( texture1, texture_coord ), texture( texture2, texture_coord ), 0.7 );
}