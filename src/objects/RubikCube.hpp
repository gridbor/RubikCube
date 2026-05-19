#pragma once
#include "renderer/Renderable.hpp"
#include "renderer/Shaders.hpp"
#include "components/Transform.hpp"
#include "components/Animation.hpp"
#include "GUI.hpp"


inline const float HS = 0.5f;
inline const glm::vec3 FRONT_COLOR{ 0.f, 0.f, 0.66f };
inline const glm::vec3 BACK_COLOR{ 0.f, 0.66f, 0.f };
inline const glm::vec3 LEFT_COLOR{ 0.66f, 0.f, 0.f };
inline const glm::vec3 RIGHT_COLOR{ 0.66f, 0.33f, 0.f };
inline const glm::vec3 TOP_COLOR{ 0.66f, 0.66f, 0.66f };
inline const glm::vec3 BOTTOM_COLOR{ 0.66f, 0.66f, 0.f };

inline const float ROTATE_TIME = 0.33f;


#include "Piece.hpp"


class RubikCube : public Renderable, public Transform, public Animation {
public:
	RubikCube()
	{
		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS,  HS), FRONT_COLOR)); // 0
		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS,  HS), FRONT_COLOR)); // 1
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS,  HS), FRONT_COLOR)); // 2
		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS,  HS), FRONT_COLOR)); // 3

		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS,  HS), RIGHT_COLOR)); // 4
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS,  HS), RIGHT_COLOR)); // 5
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS, -HS), RIGHT_COLOR)); // 6
		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS, -HS), RIGHT_COLOR)); // 7

		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS, -HS), BACK_COLOR)); // 8
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS, -HS), BACK_COLOR)); // 9
		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS, -HS), BACK_COLOR)); // 10
		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS, -HS), BACK_COLOR)); // 11

		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS, -HS), LEFT_COLOR)); // 12
		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS, -HS), LEFT_COLOR)); // 13
		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS,  HS), LEFT_COLOR)); // 14
		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS,  HS), LEFT_COLOR)); // 15

		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS,  HS), TOP_COLOR)); // 16
		//m_vertices.push_back(Vertex(glm::vec3(-HS,  HS, -HS), TOP_COLOR)); // 17
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS, -HS), TOP_COLOR)); // 18
		//m_vertices.push_back(Vertex(glm::vec3( HS,  HS,  HS), TOP_COLOR)); // 19

		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS, -HS), BOTTOM_COLOR)); // 20
		//m_vertices.push_back(Vertex(glm::vec3(-HS, -HS,  HS), BOTTOM_COLOR)); // 21
		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS,  HS), BOTTOM_COLOR)); // 22
		//m_vertices.push_back(Vertex(glm::vec3( HS, -HS, -HS), BOTTOM_COLOR)); // 23

		//m_indices = {
		//	0, 2, 1, 0, 3, 2,
		//	4, 6, 5, 4, 7, 6,
		//	8, 10, 9, 8, 11, 10,
		//	12, 14, 13, 12, 15, 14,
		//	16, 18, 17, 16, 19, 18,
		//	20, 22, 21, 20, 23, 22
		//};

		std::array<ESideFlags, 3> flags1 = { ESideFlags::F, ESideFlags::None, ESideFlags::B };
		std::array<ESideFlags, 3> flags2 = { ESideFlags::L, ESideFlags::None, ESideFlags::R };
		std::array<ESideFlags, 3> flags3 = { ESideFlags::U, ESideFlags::None, ESideFlags::D };
		for (const auto& f1 : flags1) {
			for (const auto& f2 : flags2) {
				for (const auto& f3 : flags3) {
					ESideFlags flags = f1 | f2 | f3;
					if (flags != ESideFlags::None) {
						m_pieces.emplace_back(new Piece(this, flags));
						switch (flags) {
						case ESideFlags::F:
						case ESideFlags::B:
						case ESideFlags::L:
						case ESideFlags::R:
						case ESideFlags::U:
						case ESideFlags::D:
							m_sides.insert_or_assign(flags, m_pieces.back().get());
							break;
						}
					}
				}
			}
		}
	}

	~RubikCube()
	{
		m_sides.clear();
		for (auto& piece : m_pieces) {
			if (!piece) continue;
			piece.reset();
		}
		m_pieces.clear();

		//m_indices.clear();
		//m_vertices.clear();
	}

	void Init() override
	{
		//m_indexType = GL_UNSIGNED_SHORT;
		//InitInternal<Vertex>(m_vertices, m_indices);

		//SetAttribute(0, 3, offsetof(Vertex, position));
		//SetAttribute(1, 3, offsetof(Vertex, color));

		for (auto& piece : m_pieces) {
			if (!piece) continue;
			piece->Init();
		}

		GUI::Get().AddGuiFunction("Cube_GUI", std::bind(&RubikCube::CubeGUIDraw, this));
	}

	bool EventProcess(const SDL_Event& event)
	{
		if (event.type == SDL_EVENT_KEY_DOWN) {
			Piece* sidePiece = nullptr;
			switch (event.key.key) {
			case SDLK_SPACE: StartAnimation(ROTATE_TIME, GetRotation()); break;
			case SDLK_U: sidePiece = m_sides[ESideFlags::F]; break;
			case SDLK_I: sidePiece = m_sides[ESideFlags::U]; break;
			case SDLK_O: sidePiece = m_sides[ESideFlags::B]; break;
			case SDLK_J: sidePiece = m_sides[ESideFlags::L]; break;
			case SDLK_K: sidePiece = m_sides[ESideFlags::D]; break;
			case SDLK_L: sidePiece = m_sides[ESideFlags::R]; break;
			}
			if (sidePiece && !m_sideAnimation) {
				m_sideAnimation = true;
				int index = 0;
				glm::vec3 pos = sidePiece->GetPosition();
				if (glm::epsilonEqual(pos.x, 0.f, 0.001f) && glm::epsilonNotEqual(pos.y, 0.f, 0.001f) && glm::epsilonEqual(pos.z, 0.f, 0.001f)) {
					index = 1;
				}
				else if (glm::epsilonEqual(pos.x, 0.f, 0.001f) && glm::epsilonEqual(pos.y, 0.f, 0.001f) && glm::epsilonNotEqual(pos.z, 0.f, 0.001f)) {
					index = 2;
				}
				for (const auto& piece : m_pieces) {
					if (!piece || piece.get() == sidePiece) continue;
					if (glm::epsilonEqual(pos[index], piece->GetPosition()[index], 0.001f)) {
						piece->SetAnimationSide(sidePiece);
					}
				}
				sidePiece->StartAnimation(ROTATE_TIME, sidePiece->GetMatrix(), [this, sidePiece]() {
					for (const auto& piece : m_pieces) {
						if (!piece || piece.get() == sidePiece) continue;
						piece->SetAnimationSide(nullptr);
					}
					m_sideAnimation = false;
				});
			}
		}
		return false;
	}

	void Update(float deltaTime)
	{
		if (IsAnimate()) {
			SetRotation(glm::rotate(std::any_cast<glm::quat>(m_startValue), glm::radians(90.f * m_alpha), UP));
		}
		UpdateAnimation(deltaTime);
		for (const auto& piece : m_pieces) {
			if (!piece) continue;
			piece->Update(deltaTime);
		}
	}

	void Render() override
	{
		//Shaders::Get().UpdateUniformData<ObjectData>("model", ObjectData{ .model = GetModelMatrix() });
		//Renderable::Render();
		for (auto& piece : m_pieces) {
			if (!piece) continue;
			piece->Render();
		}
	}

private:
	void CubeGUIDraw()
	{
		for (const auto& [_, side] : m_sides) {
			if (!side) continue;
			glm::vec3 pos = side->GetPosition();
			const auto& clrs = side->GetSideColors();
			ImGui::TextColored(ImVec4(clrs[0].r, clrs[0].g, clrs[0].b, 1.f), "Side_%d: (X=%f, Y=%f, Z=%f)", (int)side->GetSideFlags(), pos.x, pos.y, pos.z);
		}
	}

private:
	//std::vector<Vertex> m_vertices;
	//std::vector<uint16_t> m_indices;
	std::vector<std::unique_ptr<Piece>> m_pieces;
	std::unordered_map<ESideFlags, Piece*> m_sides;
	bool m_sideAnimation = false;

};
