#version 330 core

in vec3 colorVS;
out vec4 color;

void main()
{
    color = vec4(colorVS, 1.0f);
} 