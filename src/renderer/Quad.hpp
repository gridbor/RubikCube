#pragma once
#include "Renderable.hpp"
#include "Shaders.hpp"


class Quad : public Renderable {
public:
	Quad()
	{
		m_vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f)));
		m_vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
		m_vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
		m_vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
		m_indices = { 0, 2, 1, 0, 3, 2 };
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

		Shaders::Get().CreateUniformBuffer<ObjectData>("quad", 1);
	}

	void Render() override
	{
		Shaders::Get().UpdateUniformData<ObjectData>("quad", m_modelData);
		Renderable::Render();
	}

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
	ObjectData m_modelData{};

};
