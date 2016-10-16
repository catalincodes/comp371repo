#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorInput;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 colorVS;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
	colorVS = colorInput;
}