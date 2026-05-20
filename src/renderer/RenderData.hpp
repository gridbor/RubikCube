#pragma once
#include <glm/glm.hpp>

#include "Utils.hpp"


struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv{};
};

struct ProjectionViewData {
	glm::mat4 projection;
	glm::mat4 view;
};

struct ObjectData {
	glm::mat4 model;
};

inline const glm::vec3 UP = glm::vec3(0.f, 1.f, 0.f);
inline const glm::vec3 FRONT = glm::vec3(0.f, 0.f, -1.f);
inline const glm::vec3 RIGHT = glm::vec3(1.f, 0.f, 0.f);
