#pragma once
#include <filesystem>
#include <vector>
#include <string_view>
#include <fstream>
#include <cstring>
#include <SDL3/SDL.h>


class Assets {
public:
	Assets()
	{
		s_instance = this;
		while (!std::filesystem::exists(m_assetsPath / "assets") && m_assetsPath != m_assetsPath.root_path()) {
			m_assetsPath = m_assetsPath.parent_path();
		}
		if (m_assetsPath == m_assetsPath.root_path()) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Assets folder not found!");
		}
		else {
			m_assetsPath /= "assets";
		}
	}

	~Assets()
	{
		m_assetsPath.clear();
		s_instance = nullptr;
	}

	static inline Assets& Get() { return *s_instance; }

	std::vector<std::byte> LoadFile(std::string_view filePath) const
	{
		std::vector<std::byte> bytes;
		std::filesystem::path p = GetAssetPath(filePath);
		if (!std::filesystem::exists(p)) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File not found! %s", p.string().c_str());
		}
		else {
			std::ifstream fs(p.string(), std::ios::binary);
			if (fs.is_open()) {
				std::stringstream buffer;
				buffer << fs.rdbuf();
				std::string s = buffer.str();
				bytes.resize(s.size());
				std::memcpy(bytes.data(), s.data(), s.size());
			}
		}
		return bytes;
	}

private:
	std::filesystem::path GetAssetPath(std::string_view filePath) const
	{
		std::filesystem::path asset{ filePath };
		if (asset.is_absolute()) {
			return asset;
		}
		return m_assetsPath / asset;
	}

private:
	static inline Assets* s_instance = nullptr;
	std::filesystem::path m_assetsPath = std::filesystem::current_path();

};
