#pragma once
#include <string_view>
#include <unordered_map>
#include <glad/glad.h>

#include "assets/Assets.hpp"


class Shaders {

	struct ProgramHolder {
		GLuint pipelineId{ 0 };
		GLuint vertexProgramId{ 0 };
		GLuint fragmentProgramId{ 0 };
	};

public:
	Shaders()
	{
		s_instance = this;
	}

	~Shaders()
	{
		for (const auto& [_, ubId] : m_uniformBuffers) {
			glDeleteBuffers(1, &ubId);
		}
		m_uniformBuffers.clear();
		for (const auto& [_, programHolder] : m_programs) {
			glDeleteProgram(programHolder.vertexProgramId);
			glDeleteProgram(programHolder.fragmentProgramId);
			glDeleteProgramPipelines(1, &programHolder.pipelineId);
		}
		m_programs.clear();
		s_instance = nullptr;
	}

	static inline Shaders& Get() { return *s_instance; }

	void CreateProgramPipeline(std::string_view name, std::string_view vertexShaderPath, std::string_view fragmentShaderPath)
	{
		if (m_programs.contains(name)) {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Shader \"%s\" is already loaded!", name.data());
			return;
		}
		GLuint programPipeline = 0;
		glCreateProgramPipelines(1, &programPipeline);
		m_programs[name].pipelineId = programPipeline;
		ShaderProgramCreateAndUse(name, vertexShaderPath);
		ShaderProgramCreateAndUse(name, fragmentShaderPath);
	}

	void BindProgramPipeline(std::string_view name)
	{
		if (!m_programs.contains(name)) return;
		glBindProgramPipeline(m_programs[name].pipelineId);
	}

	template<typename T>
	void CreateUniformBuffer(std::string_view name, GLuint index, T* data = nullptr, GLbitfield flags = GL_DYNAMIC_STORAGE_BIT)
	{
		GLuint bufferId{ 0 };
		glCreateBuffers(1, &bufferId);
		glNamedBufferStorage(bufferId, sizeof(T), data, flags);
		glBindBufferBase(GL_UNIFORM_BUFFER, index, bufferId);
		m_uniformBuffers.insert_or_assign(name, bufferId);
	}

	template<typename T>
	void UpdateUniformData(std::string_view name, const T& data)
	{
		if (!m_uniformBuffers.contains(name)) return;
		glNamedBufferSubData(m_uniformBuffers[name], 0, sizeof(T), &data);
	}

private:
	void CheckProgram(GLuint programId, std::string_view shaderPath) const
	{
		GLint success = 0;
		glGetProgramiv(programId, GL_LINK_STATUS, &success);
		if (!success) {
			GLint logLength = 0;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
			std::string log;
			log.resize(logLength);
			glGetProgramInfoLog(programId, logLength, nullptr, log.data());
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader \"%s\" linking failed! %s", shaderPath.data(), log.c_str());
		}
	}

	void ShaderProgramCreateAndUse(std::string_view name, std::string_view path)
	{
		std::vector<std::byte> codeBytes = Assets::Get().LoadFile(path);
		if (codeBytes.empty()) return;
		std::string s;
		s.resize(codeBytes.size());
		std::memcpy(s.data(), codeBytes.data(), codeBytes.size());
		const char* code = s.c_str();
		GLenum type = GL_NONE;
		GLbitfield stages = GL_NONE;
		if (path.ends_with(".vert")) {
			type = GL_VERTEX_SHADER;
			stages = GL_VERTEX_SHADER_BIT;
		}
		else if (path.ends_with(".frag")) {
			type = GL_FRAGMENT_SHADER;
			stages = GL_FRAGMENT_SHADER_BIT;
		}
		if (type == GL_NONE) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Unsupported shader type!");
			return;
		}
		GLuint programId = glCreateShaderProgramv(type, 1, &code);
		CheckProgram(programId, path);
		glUseProgramStages(m_programs[name].pipelineId, stages, programId);
		if (type == GL_VERTEX_SHADER) {
			m_programs[name].vertexProgramId = programId;
		}
		else {
			m_programs[name].fragmentProgramId = programId;
		}
	}

private:
	static inline Shaders* s_instance = nullptr;
	std::unordered_map<std::string_view, ProgramHolder> m_programs;
	std::unordered_map<std::string_view, GLuint> m_uniformBuffers;

};
