#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/RenderData.hpp"


class Camera {
public:
	Camera() : Camera(glm::vec3(0.f), glm::radians(-90.f), glm::radians(0.f)) {}
	Camera(const glm::vec3& pos, float yaw, float pitch,
		float fov = glm::radians(45.f), float aspect = 4.f / 3.f, float znear = 0.1f, float zfar = 100.f):
		m_position{ pos },
		m_pitch{ pitch },
		m_yaw{ yaw }
	{
		UpdateProjection(fov, aspect, znear, zfar);
		UpdateVectors();
	}

	~Camera()
	{
	}

	ProjectionViewData& GetProjectionViewData() { return m_pvData; }

	void UpdateProjection(float fov, float aspect, float znear, float zfar)
	{
		m_pvData.projection = glm::perspective(fov, aspect, znear, zfar);
	}

	void MouseMove(float x, float y)
	{
		m_yaw += glm::radians(x * m_mouseSensitivity);
		m_pitch -= glm::radians(y * m_mouseSensitivity);
		m_pitch = glm::clamp(m_pitch, glm::radians(-89.f), glm::radians(89.f));

		UpdateVectors();
	}

	void UpdatePosition(float deltaTime)
	{
		glm::vec3 moveDirection{ 0.f };
		const bool* keyStates = SDL_GetKeyboardState(nullptr);
		if (keyStates[SDL_SCANCODE_W]) moveDirection += m_front;
		if (keyStates[SDL_SCANCODE_A]) moveDirection -= m_right;
		if (keyStates[SDL_SCANCODE_S]) moveDirection -= m_front;
		if (keyStates[SDL_SCANCODE_D]) moveDirection += m_right;
		if (keyStates[SDL_SCANCODE_Q]) moveDirection -= UP;
		if (keyStates[SDL_SCANCODE_E]) moveDirection += UP;
		if (glm::any(glm::notEqual(moveDirection, glm::vec3(0.f)))) {
			m_position += m_movementSpeed * deltaTime * moveDirection;
			UpdateView();
		}
	}

	const glm::vec3& GetPosition() const { return m_position; }

private:
	void UpdateVectors()
	{
		glm::vec3 front{};
		front.x = cosf(m_yaw) * cosf(m_pitch);
		front.y = sinf(m_pitch);
		front.z = sinf(m_yaw) * cosf(m_pitch);
		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, UP));
		m_up = glm::normalize(glm::cross(m_right, m_front));

		UpdateView();
	}

	void UpdateView()
	{
		m_pvData.view = glm::lookAt(m_position, m_position + m_front, m_up);
	}

private:
	glm::vec3 m_position;
	float m_pitch = glm::radians(0.f);
	float m_yaw = glm::radians(-90.f);
	glm::vec3 m_front = FRONT;
	glm::vec3 m_right;
	glm::vec3 m_up;
	ProjectionViewData m_pvData{};
	float m_mouseSensitivity = 0.1f;
	float m_movementSpeed = 2.f;

};
