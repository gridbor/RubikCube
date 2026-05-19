#include <iostream>
#include <memory>
#include <vector>
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "GUI.hpp"
#include "Game.hpp"


int main()
{
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int width = 800;
	int height = 600;
	SDL_Window* window = SDL_CreateWindow("Rubik's Cube", width, height, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		SDL_Quit();
		return 2;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 3;
	}

	if (!SDL_GL_MakeCurrent(window, context)) {
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 4;
	}
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	std::unique_ptr<GUI> gui = std::make_unique<GUI>(window, context);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback([](GLenum src, GLenum type, GLuint id, GLenum sev, GLsizei len, const GLchar* msg, const void* user) {
		if (type != GL_DEBUG_TYPE_OTHER) {
			std::cout << "OpenGL Log: " << msg << "\n";
		}
	}, nullptr);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	std::unique_ptr<Game> game = std::make_unique<Game>((float)width, (float)height);
	game->Init();

	Uint64 currentTicks = SDL_GetTicksNS();
	Uint64 accumulator = 0LL;
	const Uint64 fixedStep = SDL_NS_PER_SECOND / 60LL;
	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			bool prevent = gui->ProcessEvent(event);
			if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
				running = false;
			}
			else if (!prevent) {
				game->EventProcess(event);
			}
		}

		gui->NewFrame();

		Uint64 nowTicks = SDL_GetTicksNS();
		Uint64 frameTicks = nowTicks - currentTicks;
		if (frameTicks > SDL_MS_TO_NS(250LL)) frameTicks = SDL_MS_TO_NS(250LL);
		currentTicks = nowTicks;
		accumulator += frameTicks;
		while (accumulator >= fixedStep) {
			float deltaTime = (float)fixedStep / SDL_NS_PER_SECOND;
			game->Update(deltaTime);
			accumulator -= fixedStep;
		}

		glViewport(0, 0, width, height);
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game->Render();

		gui->Draw();
		gui->RenderFrame();

		SDL_GL_SwapWindow(window);
	}

	game.reset();
	gui.reset();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
