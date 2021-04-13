#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 model;
uniform mat4 vp;

out vec4 ex_Color;
out vec3 ex_Normal;
out vec4 ex_FragPos;
out vec3 lightpos;

void main(void) {
	ex_FragPos = (model * vec4(in_Position, 1.0f));
	ex_Normal = (vec4(in_Normal, 1.0f)).xyz;
	gl_Position = vp * ex_FragPos;
	ex_Color = vec4(normalize(in_Normal), 1.0);

	lightpos = (inverse(vp) * vec4(vec3(-2,2,0), 1.0)).xyz;
}
