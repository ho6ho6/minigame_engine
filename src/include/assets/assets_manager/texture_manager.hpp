/*テクスチャの読み込みを行う.png/.jpeg*/

#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER

#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include "../texture.hpp"

namespace n_texturemanager
{

	class texture_manager
	{
	public:

		texture_manager(const std::filesystem::path& base_Dir) : m_baseDir(base_Dir) {}

		// 起動時に一度だけ
		void LoadAllTextures();

		// テクスチャ名で取得
		Texture* GetTextureName(const std::string& name);

		// テクスチャ名一覧を取得
		const std::unordered_map<std::string, Texture>& GetTextureNames() const;

	private:
		void LoadTextureFromFile(const std::filesystem::path& filepath);

		std::filesystem::path m_baseDir; //"Assets/textures/"
		std::unordered_map<std::string, Texture> m_Textures;
	};
}

#endif // !TEXTURE_MANAGER