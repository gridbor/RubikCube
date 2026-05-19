#pragma once
#include "renderer/Renderable.hpp"
#include "components/Transform.hpp"
#include "components/Animation.hpp"
#include "GUI.hpp"


class TestCorner : public Renderable, public Transform, public Animation {
public:
	TestCorner()
	{
		const glm::vec3 RED_COLOR = { 1.f, 0.f, 0.f };
		const glm::vec3 GREEN_COLOR = { 0.f, 1.f, 0.f };
		const glm::vec3 BLUE_COLOR = { 0.f, 0.f, 1.f };
		const float hs = 0.3f;
		CreateQuad({ glm::vec3{-hs, -hs,  hs}, glm::vec3{-hs,  hs,  hs}, glm::vec3{ hs,  hs,  hs}, glm::vec3{ hs, -hs,  hs} }, RED_COLOR);
		CreateQuad({ glm::vec3{-hs,  hs,  hs}, glm::vec3{-hs,  hs, -hs}, glm::vec3{ hs,  hs, -hs}, glm::vec3{ hs,  hs,  hs} }, GREEN_COLOR);
		CreateQuad({ glm::vec3{ hs, -hs,  hs}, glm::vec3{ hs,  hs,  hs}, glm::vec3{ hs,  hs, -hs}, glm::vec3{ hs, -hs, -hs} }, BLUE_COLOR);
		SetPosition(glm::vec3(2.f));
		m_objectData.model = GetModelMatrix();
	}

	~TestCorner()
	{
		m_indices.clear();
		m_vertices.clear();
	}

	void Init()
	{
		m_indexType = GL_UNSIGNED_SHORT;
		InitInternal<Vertex>(m_vertices, m_indices);

		SetAttribute(0, 3, offsetof(Vertex, position));
		SetAttribute(1, 3, offsetof(Vertex, color));

		GUI::Get().AddGuiFunction("test_corner", std::bind(&TestCorner::GuiDraw, this));
	}

	void Update(float deltaTime)
	{
		if (IsAnimate()) {
			glm::mat4 mat = glm::mat4(1.f);
			mat = glm::translate(mat, glm::vec3(0.f, 2.f, 0.f));
			mat = glm::rotate(mat, glm::radians(90.f * m_alpha), UP);
			mat = glm::translate(mat, -glm::vec3(0.f, 2.f, 0.f));
			m_matrix = mat * std::any_cast<glm::mat4>(m_startValue);
		}
		UpdateAnimation(deltaTime);
	}

	void Render() override
	{
		m_objectData.model = GetModelMatrix();
		Shaders::Get().UpdateUniformData("model", m_objectData);
		Renderable::Render();
	}

private:
	void CreateQuad(const std::array<glm::vec3, 4>& vs, const glm::vec3& c)
	{
		uint16_t index = (uint16_t)m_vertices.size();
		m_vertices.push_back(Vertex(vs[0], c));
		m_vertices.push_back(Vertex(vs[1], c));
		m_vertices.push_back(Vertex(vs[2], c));
		m_vertices.push_back(Vertex(vs[3], c));
		m_indices.insert(m_indices.end(), { index, (uint16_t)(index + 2), (uint16_t)(index + 1), index, (uint16_t)(index + 3), (uint16_t)(index + 2) });
	}

	void GuiDraw()
	{
		glm::vec3 pos = GetPosition();
		glm::quat rot = GetRotation();
		if (ImGui::Button("Rotate")) {
			if (m_onAnimation) return;
			m_onAnimation = true;
			StartAnimation(1.f, m_matrix, [=]() {
				m_onAnimation = false;
			});
		}
		glm::vec3 euler = glm::degrees(glm::eulerAngles(rot));
		ImGui::Text("Position: (%f, %f, %f)\nRotation (Quat): (X=%f, Y=%f, Z=%f, W=%f)\n        (Euler): (Pitch=%f, Yaw=%f, Roll=%f)",
			pos.x, pos.y, pos.z,
			rot.x, rot.y, rot.z, rot.w,
			euler.x, euler.y, euler.z
		);
	}

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
	ObjectData m_objectData{};
	bool m_onAnimation = false;

};
