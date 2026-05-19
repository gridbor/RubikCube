#pragma once
#include <any>
#include <functional>
#include <glm/glm.hpp>


enum class EAnimState {
	Idle = 0,
	Busy
};


class Animation {
protected:
	Animation() = default;

public:
	virtual ~Animation() = default;

	bool IsAnimate() const { return m_state == EAnimState::Busy; }

	template<typename T>
	void StartAnimation(float duration, const T& startValue, const std::function<void()>& endCallback = nullptr)
	{
		if (IsAnimate()) return;
		m_startValue = startValue;
		m_alpha = 0.f;
		m_cur = 0.f;
		m_end = duration;
		m_state = EAnimState::Busy;
		m_endCallback = endCallback;
	}

protected:
	virtual void UpdateAnimation(float deltaTime)
	{
		if (m_state == EAnimState::Busy) {
			m_cur += deltaTime;
			if (m_cur >= m_end) {
				m_cur = m_end;
				if (glm::epsilonEqual<float>(m_alpha, 1.f, SDL_FLT_EPSILON)) {
					m_state = EAnimState::Idle;
					if (m_endCallback) m_endCallback();
				}
			}
			m_alpha = m_cur / m_end;
		}
	}

protected:
	EAnimState m_state = EAnimState::Idle;
	float m_alpha = 0.f;
	float m_cur = 0.f;
	float m_end = 0.f;
	std::any m_startValue;
	std::function<void()> m_endCallback;

};
