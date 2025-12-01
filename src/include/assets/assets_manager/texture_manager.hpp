/*テクスチャの読み込みを行う.png/.jpeg*/

#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER

#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include "include/assets/texture.hpp"

namespace n_texturemanager
{
	class texture_manager
	{
	public:

		texture_manager(const std::filesystem::path& base_Dir) : m_baseDir(base_Dir) {}
		~texture_manager();

		// 起動時に一度だけ
		void LoadAllTextures();

		// テクスチャ名で取得 1つのみ
		Texture* GetTextureName(const std::string& name);

		// テクスチャ名一覧を取得 すべて
		const std::unordered_map<std::string, Texture>& GetTextureNames() const;

		// テクスチャキー一覧を取得
		std::vector<std::string> GetTextureKeys() const;

	private:
		// テクスチャを1つ読み込む
		void LoadTextureFromFile(const std::filesystem::path& filepath);

		std::filesystem::path m_baseDir; //テクニックファイルを参照する baseDir "Assets/textures/"
		std::unordered_map<std::string, Texture> m_Textures;
	};

	// グローバルインスタンス
	extern texture_manager instance_texmag;
}

#endif // !TEXTURE_MANAGER