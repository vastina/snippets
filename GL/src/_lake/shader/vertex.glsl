#version 450 core
layout( location = 0 ) in vec2 aPos;
out vec2 vUv;

void main()
{
  gl_Position = vec4( aPos, 0.0, 1.0 );
  vUv = aPos * 0.5 + 0.5; // 转换为[0,1]范围
}