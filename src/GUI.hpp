#pragma once
#include <functional>
#include <unordered_map>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>


class GUI {
public:
	GUI(SDL_Window* window, void* glContext)
	{
		s_instance = this;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplSDL3_InitForOpenGL(window, glContext);
		ImGui_ImplOpenGL3_Init("#version 420 core");
	}

	~GUI()
	{
		m_functions.clear();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		s_instance = nullptr;
	}

	static inline GUI& Get() { return *s_instance; }

	bool ProcessEvent(const SDL_Event& event)
	{
		bool prevent = false;
		ImGui_ImplSDL3_ProcessEvent(&event);
		ImGuiIO& io = ImGui::GetIO();
		prevent = io.WantCaptureKeyboard || io.WantCaptureMouse;
		return prevent;
	}

	void NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void Draw()
	{
		if (m_guiDisabled || m_functions.empty()) return;

		ImGui::Begin("GUI");

		for (const auto& [_, func] : m_functions) {
			if (func) func();
		}

		ImGui::End();
	}

	void RenderFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void AddGuiFunction(const std::string_view& name, std::function<void()> func)
	{
		if (HasGuiFunction(name)) {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ImGui function \"%s\" already exists! Function REPLACED", name.data());
		}
		m_functions.insert_or_assign(name, func);
	}
	void RemoveGuiFunction(const std::string_view& name)
	{
		if (!HasGuiFunction(name)) return;
		m_functions.erase(name);
	}
	bool HasGuiFunction(const std::string_view& name) const { return m_functions.contains(name); }

	void Disable() { m_guiDisabled = true; }
	void Enable() { m_guiDisabled = false; }

private:
	static inline GUI* s_instance = nullptr;

	std::unordered_map<std::string_view, std::function<void()>> m_functions;
	bool m_guiDisabled = false;

};
