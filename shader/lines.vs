#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec4 in_Color;

uniform float zoom;
uniform vec2 center;

out vec4 ex_Color;

void main(){

  vec2 pos = in_Position.xy*zoom - center;
  gl_Position = vec4(pos.x, -pos.y, -1.0, 1.0);

  ex_Color = in_Color;

}
