/*
Iterative Circumcenter Map
Vertex Shader
Author: Nicholas McDonald, 2021-2022
*/

#version 430 core

layout(location = 0) in vec3 in_Position;

uniform mat4 vp;

flat out vec3 lightpos;
out vec3 ex_Normal;

void main(void) {

	gl_Position = vp * vec4(in_Position, 1.0f);
	ex_Normal = normalize(in_Position);

	lightpos = normalize((inverse(vp) * vec4(-1.0, 1.0, -1.0, 1.0)).xyz);

}
