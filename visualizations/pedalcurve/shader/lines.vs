#version 430 core

layout (location = 0) in vec2 in_Position;

uniform float zoom;
uniform vec2 center;

void main(){

  vec2 pos = in_Position*zoom - center;
  gl_Position = vec4(pos.x, pos.y, -1.0, 1.0);

}
