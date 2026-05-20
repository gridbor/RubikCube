#pragma once
#include <glm/glm.hpp>


struct AABB {
	glm::vec3 min;
	glm::vec3 max;
};


class Collision {
protected:
	Collision() = default;

public:
	virtual ~Collision() = default;

	const AABB& GetBoundingBox() const { return m_bounds; }

protected:
	void UpdateBounds(const glm::mat4& matrix)
	{
		m_bounds.min = glm::vec3(matrix[3]) - HS;
		m_bounds.max = glm::vec3(matrix[3]) + HS;
	}

protected:
	AABB m_bounds{};
	bool m_boundsNeedUpdate = false;

};
