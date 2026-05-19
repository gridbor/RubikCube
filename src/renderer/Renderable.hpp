#pragma once
#include <vector>
#include <glad/glad.h>

#include "RenderData.hpp"


class Renderable {
protected:
	Renderable()
	{
		glCreateVertexArrays(1, &m_vao);
		glCreateBuffers(1, &m_vbo);
		glCreateBuffers(1, &m_ebo);
	}

public:
	virtual ~Renderable()
	{
		glDeleteBuffers(1, &m_ebo);
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}

	virtual void Init() = 0;

	virtual void Render()
	{
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_indicesCount, m_indexType, nullptr);
	}

protected:
	template<typename T, typename U = uint16_t>
	void InitInternal(const std::vector<T>& vertices, const std::vector<U>& indices)
	{
		m_indicesCount = indices.size();
		glNamedBufferStorage(m_vbo, vertices.size() * sizeof(T), vertices.data(), 0);
		glNamedBufferStorage(m_ebo, indices.size() * sizeof(U), indices.data(), 0);
		glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(T));
		glVertexArrayElementBuffer(m_vao, m_ebo);
	}

	void SetAttribute(GLuint index, GLint count, GLuint offset, GLuint binding = 0) const
	{
		glEnableVertexArrayAttrib(m_vao, index);
		glVertexArrayAttribFormat(m_vao, index, count, GL_FLOAT, GL_FALSE, offset);
		glVertexArrayAttribBinding(m_vao, index, binding);
	}

protected:
	GLuint m_vao{ 0 };
	GLuint m_vbo{ 0 };
	GLuint m_ebo{ 0 };
	GLsizei m_indicesCount{ 0 };
	GLenum m_indexType{ 0 };
};
