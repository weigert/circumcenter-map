#version 430 core

in vec4 ex_Color;

uniform vec3 color;

out vec4 fragColor;

void main(void) {

  fragColor = ex_Color;

}
