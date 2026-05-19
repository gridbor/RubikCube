#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Transform {
protected:
	Transform() : Transform(glm::vec3(0.f), glm::quat(glm::vec3(0.f)), glm::vec3(1.f)) {}
	Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) : m_scale{ scale }
	{
		m_matrix = glm::translate(m_matrix, pos);
		m_matrix = m_matrix * glm::mat4_cast(rot);
	}

public:
	virtual ~Transform()
	{
	}

	glm::vec3 GetPosition() const { return glm::vec3(m_matrix[3]); }
	void SetPosition(const glm::vec3& pos) { m_matrix[3] = glm::vec4(pos, 1.f); }
	glm::quat GetRotation() const { return glm::quat(m_matrix); }
	void SetRotation(const glm::quat& rot)
	{
		glm::vec4 pos = m_matrix[3];
		m_matrix = glm::mat4_cast(rot);
		m_matrix[3] = pos;
	}
	glm::vec3 GetScale() const { return m_scale; }
	void SetScale(const glm::vec3& scale) { m_scale = scale; }

	glm::mat4 GetModelMatrix() { return m_matrix * glm::scale(glm::mat4(1.f), m_scale); }
	glm::mat4 GetMatrix() { return m_matrix; }

protected:
	glm::mat4 m_matrix = glm::mat4(1.f);
	glm::vec3 m_scale = glm::vec3(1.f);

};
