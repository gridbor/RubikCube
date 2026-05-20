#version 460 core

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec2 vUV;
layout(location = 0) out vec4 ResultColor;

void main()
{
	float edge = 0.03;
	float left = smoothstep(0.0, edge, vUV.x);
	float right = smoothstep(1.0, 1.0 - edge, vUV.x);
	float bottom = smoothstep(0.0, edge, vUV.y);
	float top = smoothstep(1.0, 1.0 - edge, vUV.y);
	float factor = left * right * bottom * top;
	vec3 cc = vColor * factor;
	ResultColor = vec4(cc, 1.0);
}
