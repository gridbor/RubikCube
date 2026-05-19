#version 460 core

layout(location = 0) in vec3 vColor;
layout(location = 0) out vec4 ResultColor;

void main()
{
	ResultColor = vec4(vColor, 1.0);
}
