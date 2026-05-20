#pragma once
#include "Camera.hpp"
#include "components/Collision.hpp"

#include <optional>


class Ray {
public:
	Ray(const glm::vec3& o, const glm::vec3& d):
		origin{ o }, direction{ d }
	{ }
	~Ray() {}

	std::optional<float> IntersectAABB(const AABB& box) const
	{
		glm::vec3 tmin = (box.min - origin) / direction;
		glm::vec3 tmax = (box.max - origin) / direction;
		glm::vec3 t1 = glm::min(tmin, tmax);
		glm::vec3 t2 = glm::max(tmin, tmax);
		float nearMax = glm::max(glm::max(t1.x, t1.y), t1.z);
		float farMin = glm::min(glm::min(t2.x, t2.y), t2.z);
		if (nearMax > farMin || farMin < 0.f) return std::nullopt;
		return nearMax;
	}

	std::optional<float> IntersectPlane(const glm::vec3& planePoint, const glm::vec3& planeNormal)
	{
		float denom = glm::dot(planeNormal, direction);
		if (glm::epsilonEqual(glm::abs(denom), 0.f, std::numeric_limits<float>::epsilon())) {
			return std::nullopt;
		}
		glm::vec3 pointDist = planePoint - origin;
		float dist = glm::dot(pointDist, planeNormal) / denom;
		if (dist < 0.f) {
			return std::nullopt;
		}
		return dist;
	}

	glm::vec3 origin;
	glm::vec3 direction;
};


class Raycast {
public:
	Raycast(Camera* camera, float screenWidth, float screenHeight):
		m_camera{ camera },
		m_screenWidth{ screenWidth },
		m_screenHeight{ screenHeight }
	{
		s_instance = this;
	}

	~Raycast()
	{
		m_camera = nullptr;
		s_instance = nullptr;
	}

	static inline Raycast& Get() { return *s_instance; }

	Ray MakeCameraRay(float x, float y)
	{
		glm::vec2 npos = GetNormalizedPoint(x, y);
		glm::vec4 rayClip = glm::vec4(npos, -1.f, 1.f);
		ProjectionViewData pvData = m_camera->GetProjectionViewData();
		glm::vec4 rayEye = glm::vec4(glm::vec2(glm::inverse(pvData.projection) * rayClip), -1.f, 0.f);
		glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(pvData.view) * rayEye));
		return Ray(m_camera->GetPosition(), rayWorld);
	}

private:
	glm::vec2 GetNormalizedPoint(float x, float y) const { return glm::vec2((2.f * x) / m_screenWidth - 1.f, 1.f - (2.f * y) / m_screenHeight); }

private:
	static inline Raycast* s_instance = nullptr;
	Camera* m_camera = nullptr;
	float m_screenWidth;
	float m_screenHeight;

};
