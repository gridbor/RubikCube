#pragma once
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
static inline float g_rotateDeg = 90.f;


#include "Piece.hpp"


class RubikCube : public Transform, public Animation {
public:
	RubikCube()
	{
		m_pieces.reserve(26);
		m_sides.reserve(6);
		m_animatedPieces.reserve(9);
		std::array<ESideFlags, 3> flags1 = { ESideFlags::F, ESideFlags::None, ESideFlags::B };
		std::array<ESideFlags, 3> flags2 = { ESideFlags::L, ESideFlags::None, ESideFlags::R };
		std::array<ESideFlags, 3> flags3 = { ESideFlags::U, ESideFlags::None, ESideFlags::D };
		for (const auto& f1 : flags1) {
			for (const auto& f2 : flags2) {
				for (const auto& f3 : flags3) {
					ESideFlags flags = f1 | f2 | f3;
					if (flags == ESideFlags::None) continue;
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

	~RubikCube()
	{
		m_animatedPieces.clear();
		m_sides.clear();
		for (auto& piece : m_pieces) {
			if (!piece) continue;
			piece.reset();
		}
		m_pieces.clear();
	}

	void Init()
	{
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
			case SDLK_KP_5:
			case SDLK_U: sidePiece = m_sides[ESideFlags::F]; break;
			case SDLK_KP_8:
			case SDLK_I: sidePiece = m_sides[ESideFlags::U]; break;
			case SDLK_KP_0:
			case SDLK_O: sidePiece = m_sides[ESideFlags::B]; break;
			case SDLK_KP_4:
			case SDLK_J: sidePiece = m_sides[ESideFlags::L]; break;
			case SDLK_KP_2:
			case SDLK_K: sidePiece = m_sides[ESideFlags::D]; break;
			case SDLK_KP_6:
			case SDLK_L: sidePiece = m_sides[ESideFlags::R]; break;
			}
			if (sidePiece && !m_sideAnimation) {
				g_rotateDeg = event.key.mod & SDL_KMOD_CTRL ? -90.f : 90.f;
				m_sideAnimation = true;
				int index = 0;
				glm::vec3 pos = sidePiece->GetPosition();
				if (glm::epsilonEqual(pos.x, 0.f, 0.001f) && glm::epsilonNotEqual(pos.y, 0.f, 0.001f) && glm::epsilonEqual(pos.z, 0.f, 0.001f)) {
					index = 1;
				}
				else if (glm::epsilonEqual(pos.x, 0.f, 0.001f) && glm::epsilonEqual(pos.y, 0.f, 0.001f) && glm::epsilonNotEqual(pos.z, 0.f, 0.001f)) {
					index = 2;
				}
				m_animatedPieces.clear();
				for (const auto& piece : m_pieces) {
					if (!piece || piece.get() == sidePiece) continue;
					if (glm::epsilonEqual(pos[index], piece->GetPosition()[index], 0.001f)) {
						piece->SetAnimationSide(sidePiece);
						m_animatedPieces.push_back(piece.get());
					}
				}
				m_animatedPieces.push_back(sidePiece);
				sidePiece->StartAnimation(ROTATE_TIME, sidePiece->GetMatrix(), [this, sidePiece]() {
					for (const auto& piece : m_animatedPieces) {
						if (!piece || piece == sidePiece) continue;
						piece->SetAnimationSide(nullptr);
					}
					m_animatedPieces.clear();
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
		for (const auto& piece : m_animatedPieces) {
			if (!piece) continue;
			piece->Update(deltaTime);
		}
	}

	void Render()
	{
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
		ImGui::SeparatorText("[ Matrices ]");
		for (size_t i = 0; i < m_pieces.size(); i++) {
			if (!m_pieces[i]) continue;
			ImGui::Text("(%d) Piece_%d:\n%s", i + 1, (int)m_pieces[i]->GetSideFlags(), m_pieces[i]->GetMatrixInfo().c_str());
			ImGui::NewLine();
		}
		ImGui::Separator();
	}

private:
	std::vector<std::unique_ptr<Piece>> m_pieces;
	std::unordered_map<ESideFlags, Piece*> m_sides;
	std::vector<Piece*> m_animatedPieces;
	bool m_sideAnimation = false;

};
