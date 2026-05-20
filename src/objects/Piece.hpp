#pragma once
#include "renderer/Renderable.hpp"
#include "components/Transform.hpp"
#include "components/Animation.hpp"
#include "components/Collision.hpp"
#include "GUI.hpp"

#include <array>


enum class ESideFlags : unsigned char {
	None = 0,
	F = 1 << 1,
	B = 1 << 2,
	L = 1 << 3,
	R = 1 << 4,
	U = 1 << 5,
	D = 1 << 6
};
BITMASK_ENUM(ESideFlags)


class Piece : public Renderable, public Transform, public Animation, public Collision {
public:
	Piece(Transform* parent, ESideFlags flags) : m_parent{ parent }, m_sideFlags { flags }
	{
		std::array<glm::vec3, 3> offset{};
		if ((flags & ESideFlags::F) != ESideFlags::None) {
			CreateQuad({
				glm::vec3(-HS, -HS,  HS),
				glm::vec3(-HS,  HS,  HS),
				glm::vec3( HS,  HS,  HS),
				glm::vec3( HS, -HS,  HS)
				}, FRONT_COLOR);
			offset[0] = glm::vec3(0.f, 0.f, HS * MUL);
		}
		else if ((flags & ESideFlags::B) != ESideFlags::None) {
			CreateQuad({
				glm::vec3( HS, -HS, -HS),
				glm::vec3( HS,  HS, -HS),
				glm::vec3(-HS,  HS, -HS),
				glm::vec3(-HS, -HS, -HS)
				}, BACK_COLOR);
			offset[0] = glm::vec3(0.f, 0.f, -HS * MUL);
		}

		if ((flags & ESideFlags::L) != ESideFlags::None) {
			CreateQuad({
				glm::vec3(-HS, -HS, -HS),
				glm::vec3(-HS,  HS, -HS),
				glm::vec3(-HS,  HS,  HS),
				glm::vec3(-HS, -HS,  HS)
				}, LEFT_COLOR);
			offset[1] = glm::vec3(-HS * MUL, 0.f, 0.f);
		}
		else if ((flags & ESideFlags::R) != ESideFlags::None) {
			CreateQuad({
				glm::vec3( HS, -HS,  HS),
				glm::vec3( HS,  HS,  HS),
				glm::vec3( HS,  HS, -HS),
				glm::vec3( HS, -HS, -HS)
				}, RIGHT_COLOR);
			offset[1] = glm::vec3(HS * MUL, 0.f, 0.f);
		}

		if ((flags & ESideFlags::U) != ESideFlags::None) {
			CreateQuad({
				glm::vec3(-HS,  HS,  HS),
				glm::vec3(-HS,  HS, -HS),
				glm::vec3( HS,  HS, -HS),
				glm::vec3( HS,  HS,  HS)
				}, TOP_COLOR);
			offset[2] = glm::vec3(0.f, HS * MUL, 0.f);
		}
		else if ((flags & ESideFlags::D) != ESideFlags::None) {
			CreateQuad({
				glm::vec3(-HS, -HS, -HS),
				glm::vec3(-HS, -HS,  HS),
				glm::vec3( HS, -HS,  HS),
				glm::vec3( HS, -HS, -HS)
				}, BOTTOM_COLOR);
			offset[2] = glm::vec3(0.f, -HS * MUL, 0.f);
		}

		glm::vec3 pos = offset[0] + offset[1] + offset[2];
		SetPosition(pos);
		m_bounds.min = pos - HS;
		m_bounds.max = pos + HS;
	}

	~Piece()
	{
		m_indices.clear();
		m_vertices.clear();
		m_parent = nullptr;
	}

	void Init() override
	{
		m_indexType = GL_UNSIGNED_SHORT;
		InitInternal<Vertex>(m_vertices, m_indices);

		SetAttribute(0, 3, offsetof(Vertex, position));
		SetAttribute(1, 3, offsetof(Vertex, color));
		SetAttribute(2, 2, offsetof(Vertex, uv));

		if ((m_sideFlags & ESideFlags::L) != ESideFlags::None && (m_sideFlags & ESideFlags::U) != ESideFlags::None && (m_sideFlags & ESideFlags::F) != ESideFlags::None) {
			GUI::Get().AddGuiFunction("PieceNearestCorner", [=]() {
				ImGui::Text("Bounds:\n    min = (%f, %f, %f)\n    max = (%f, %f, %f)", m_bounds.min.x, m_bounds.min.y, m_bounds.min.z, m_bounds.max.x, m_bounds.max.y, m_bounds.max.z);
			});
		}
	}

	void SetAnimationSide(Piece* sideParent)
	{
		if (m_sideParent && !sideParent) {
			SideRotateAnimation(m_sideParent->m_sideFlags, 1.f, m_sideParent->GetPosition());
			m_startValue.reset();
			m_boundsNeedUpdate = true;
		}
		else if (sideParent) {
			m_startValue = m_matrix;
		}
		m_sideParent = sideParent;
	}

	void Update(float deltaTime)
	{
		if (IsAnimate()) {
			SideRotateAnimation(m_sideFlags, m_alpha, glm::vec3(std::any_cast<glm::mat4>(m_startValue)[3]));
		}
		else if (m_sideParent) {
			SideRotateAnimation(m_sideParent->m_sideFlags, m_sideParent->m_alpha, m_sideParent->GetPosition());
		}
		UpdateAnimation(deltaTime);
	}

	void Render() override
	{
		m_objectData.model = m_parent->GetModelMatrix() * GetModelMatrix();
		if (m_boundsNeedUpdate) {
			UpdateBounds(m_objectData.model);
			m_boundsNeedUpdate = false;
		}
		Shaders::Get().UpdateUniformData<ObjectData>("model", m_objectData);
		if (m_visible) Renderable::Render();
	}

	void RefreshBounds() { m_boundsNeedUpdate = true; }

	const ESideFlags& GetSideFlags() const { return m_sideFlags; }
	const std::vector<glm::vec3>& GetSideColors() const { return m_sideColors; }

	std::string GetMatrixInfo()
	{
		std::string matStr = "[ ";
		for (int i = 0; i < m_matrix.length(); i++) {
			for (int j = 0; j < m_matrix[i].length(); j++) {
				matStr += (j == 0 ? "" : ", ") + std::to_string(m_matrix[i][j]);
			}
			matStr += (i != 3 ? "\n  " : " ]");
		}
		return matStr;
	}

	bool IsCenter() const
	{
		if ((m_sideFlags & ~ESideFlags::F) == ESideFlags::None
			|| (m_sideFlags & ~ESideFlags::B) == ESideFlags::None
			|| (m_sideFlags & ~ESideFlags::L) == ESideFlags::None
			|| (m_sideFlags & ~ESideFlags::R) == ESideFlags::None
			|| (m_sideFlags & ~ESideFlags::U) == ESideFlags::None
			|| (m_sideFlags & ~ESideFlags::D) == ESideFlags::None) {
			return true;
		}
		return false;
	}

	void ToggleVisibility() { m_visible = !m_visible; }

private:
	void CreateQuad(const std::array<glm::vec3, 4>& vs, const glm::vec3& c)
	{
		uint16_t i = (uint16_t)m_vertices.size();
		m_vertices.push_back(Vertex(vs[0], c, glm::vec2(0.f, 0.f)));
		m_vertices.push_back(Vertex(vs[1], c, glm::vec2(0.f, 1.f)));
		m_vertices.push_back(Vertex(vs[2], c, glm::vec2(1.f, 1.f)));
		m_vertices.push_back(Vertex(vs[3], c, glm::vec2(1.f, 0.f)));
		m_indices.insert(m_indices.end(), { i, (uint16_t)(i + 2), (uint16_t)(i + 1), i, (uint16_t)(i + 3), (uint16_t)(i + 2) });

		m_sideColors.push_back(c);
	}

	void SideRotateAnimation(ESideFlags flags, float alpha, glm::vec3 offset)
	{
		glm::vec3 axis = UP;
		if ((flags & ESideFlags::F) == ESideFlags::F || (flags & ESideFlags::B) == ESideFlags::B) {
			axis = FRONT;
		}
		else if ((flags & ESideFlags::L) == ESideFlags::L || (flags & ESideFlags::R) == ESideFlags::R) {
			axis = RIGHT;
		}
		glm::mat4 matStart = std::any_cast<glm::mat4>(m_startValue);
		glm::mat4 mat = glm::mat4(1.f);
		mat = glm::translate(mat, offset);
		mat = glm::rotate(mat, glm::radians(g_rotateDeg * alpha), axis);
		mat = glm::translate(mat, -offset);
		m_matrix = mat * matStart;
	}

private:
	ESideFlags m_sideFlags = ESideFlags::None;
	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
	ObjectData m_objectData{};
	Transform* m_parent = nullptr;
	Piece* m_sideParent = nullptr;
	std::vector<glm::vec3> m_sideColors;
	bool m_visible = true;

};
