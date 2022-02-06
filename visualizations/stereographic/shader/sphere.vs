/*
Iterative Circumcenter Map
Vertex Shader
Author: Nicholas McDonald, 2021-2022
*/

#version 430 core

layout(location = 0) in vec3 in_Position;

uniform mat4 vp;
uniform mat4 invvp;
uniform bool scalesphere;

uniform float scale;

flat out vec3 lightpos;
out vec3 ex_Normal;

#include transform_double.cs

void main() {

	vec3 pos = in_Position;

	if(scalesphere)
	pos /= getScale(stereographic(pos, scale));

	gl_Position = vp * vec4(pos, 1.0f);
	ex_Normal = normalize(in_Position);

	lightpos = normalize((invvp * vec4(-1.0, 1.0, -1.0, 1.0)).xyz);

}
