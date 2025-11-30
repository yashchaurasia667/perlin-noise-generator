#version 330 core

layout (location = 0) in vec2 aPos;     // quad positions
layout (location = 1) in vec2 aTex;     // UVs

out vec2 TexCoords;

void main()
{
  TexCoords = aTex;
  gl_Position = vec4(aPos, 0.0, 1.0);
}
