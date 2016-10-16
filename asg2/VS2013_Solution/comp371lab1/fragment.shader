#version 330 core

in vec3 colorFromVertexShader;
out vec4 color;

void main()
{
	color = vec4(colorFromVertexShader, 1.0f);
}