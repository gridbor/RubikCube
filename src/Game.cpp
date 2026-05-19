#include "Game.hpp"
#include "assets/Assets.hpp"
#include "renderer/Shaders.hpp"
#include "objects/Camera.hpp"
#include "objects/RubikCube.hpp"
//#include "objects/TestCorner.hpp"
#include "GUI.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


Game::Game(float w, float h):
	m_width{ w },
	m_height{ h }
{
	s_instance = this;
	m_assets = std::make_unique<Assets>();
	m_shaders = std::make_unique<Shaders>();
	m_camera = std::make_unique<Camera>(glm::vec3(-5.f, 5.f, 5.f), glm::radians(-45.f), glm::radians(-37.f));
	m_cube = std::make_unique<RubikCube>();
	//m_test = std::make_unique<TestCorner>();
	GUI::Get().AddGuiFunction("KeysInfo", []() {
		ImGui::Text("WASD - Camera move\nE/Q - Camera Up/Down");
		ImGui::Text("U - BLUE side rotate\nI - WHITE side rotate\nO - GREEN side rotate\nJ - RED side rotate\nK - YELLOW side rotate\nL - ORANGE side rotate\n");
	});
}

Game::~Game()
{
	//m_test.reset();
	m_cube.reset();
	m_camera.reset();
	m_shaders.reset();
	m_assets.reset();
	s_instance = nullptr;
}

void Game::Init()
{
	m_shaders->CreateProgramPipeline("default", "shaders/colored.vert", "shaders/colored.frag");
	m_shaders->CreateUniformBuffer<ProjectionViewData>("PV", 0, &m_camera->GetProjectionViewData());
	m_shaders->CreateUniformBuffer<ObjectData>("model", 1);
	m_cube->Init();
	//m_test->Init();
}

void Game::EventProcess(const SDL_Event& event)
{
	if (m_cube->EventProcess(event)) return;
	if (event.type == SDL_EVENT_MOUSE_MOTION && (event.motion.state & SDL_BUTTON_LMASK)) {
 		m_camera->MouseMove(event.motion.xrel, event.motion.yrel);
	}
}

void Game::Update(float deltaTime)
{
	m_camera->UpdatePosition(deltaTime);
	m_cube->Update(deltaTime);
	//m_test->Update(deltaTime);
}

void Game::Render()
{
	m_shaders->BindProgramPipeline("default");
	m_shaders->UpdateUniformData<ProjectionViewData>("PV", m_camera->GetProjectionViewData());
	m_cube->Render();
	//m_test->Render();
}
