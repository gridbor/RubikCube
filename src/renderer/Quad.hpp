#pragma once
#include "Renderable.hpp"
#include "Shaders.hpp"


class Quad : public Renderable {
public:
	Quad()
	{
		m_vertices.push_back(Vertex(glm::vec3(-20.f, -5.f, -20.f), glm::vec3(0.20f, 0.70f, 0.20f), glm::vec2(0.f, 0.f)));
		m_vertices.push_back(Vertex(glm::vec3(-20.f, -5.f,  20.f), glm::vec3(0.08f, 0.08f, 0.34f), glm::vec2(0.f, 1.f)));
		m_vertices.push_back(Vertex(glm::vec3( 20.f, -5.f,  20.f), glm::vec3(0.08f, 0.08f, 0.34f), glm::vec2(1.f, 1.f)));
		m_vertices.push_back(Vertex(glm::vec3( 20.f, -5.f, -20.f), glm::vec3(0.20f, 0.70f, 0.20f), glm::vec2(1.f, 0.f)));
		m_indices = { 0, 1, 2, 0, 2, 3 };
		m_modelData.model = glm::mat4(1.f);
	}
	~Quad()
	{
		m_indices.clear();
		m_vertices.clear();
	}

	void Init() override
	{
		m_indexType = GL_UNSIGNED_SHORT;
		InitInternal<Vertex>(m_vertices, m_indices);

		SetAttribute(0, 3, offsetof(Vertex, position));
		SetAttribute(1, 3, offsetof(Vertex, color));
		SetAttribute(2, 2, offsetof(Vertex, uv));
	}

	void Render() override
	{
		Shaders::Get().UpdateUniformData<ObjectData>("model", m_modelData);
		Renderable::Render();
	}

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
	ObjectData m_modelData{};

};
