#version 420 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;

layout(location = 0) out vec3 vColor;
layout(location = 1) out vec2 vUV;

layout(binding = 0, std140) uniform ProjectionViewData {
	mat4 projection;
	mat4 view;
};
layout(binding = 1, std140) uniform ObjectData {
	mat4 model;
};

out gl_PerVertex {
	vec4 gl_Position;
};

void main()
{
	vColor = aColor;
	vUV = aUV;
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
