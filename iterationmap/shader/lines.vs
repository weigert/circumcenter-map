#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec4 in_Color;

uniform float zoom;
uniform vec2 center;

uniform vec2 stretch;
uniform vec2 skew;

out vec4 ex_Color;

void main(){

  mat3 affine = mat3(stretch.x, skew.x, 0, skew.y, stretch.y, 0, 0, 0, 1);
  vec2 pos = in_Position.xy*zoom;

  pos = (affine*vec3(pos, 1)).xy - center;

  gl_Position = vec4(pos.x, pos.y, -1.0, 1.0);

  ex_Color = in_Color;

}
