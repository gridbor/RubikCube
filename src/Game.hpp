#pragma once
#include <memory>
#include <SDL3/SDL.h>

#include "Utils.hpp"


class Assets;
class Shaders;
class RubikCube;
class Camera;
class Raycast;


class Game {
public:
	Game(float w, float h);
	~Game();

	static inline Game& Get() { return *s_instance; }

	void Init();
	void EventProcess(const SDL_Event& event);
	void Update(float deltaTime);
	void Render();

private:
	static inline Game* s_instance = nullptr;
	float m_width = 0.f;
	float m_height = 0.f;
	std::unique_ptr<Assets> m_assets;
	std::unique_ptr<Shaders> m_shaders;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<RubikCube> m_cube;
	std::unique_ptr<Raycast> m_raycast;

};
