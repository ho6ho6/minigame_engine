#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include <imgui.h>
#include "window_base.hpp"
#include "window_scene_sprite.hpp"
#include <vector>
#include <iostream>
#include <array>

namespace n_windowscene
{
	class window_scene : public n_windowbase::window_base
	{
	public:
		void Render() override;
		void AddAssetToScene(Texture* tex, const std::string& asset_name, ImVec2 guiLocalPos, ImVec2 guiWindowPos);
		void DeleteAssetFromScene(uint64_t id);
		uint64_t GenerateUniqueSpriteId();	// ユニークID生成
		bool GetSpritePosition(int64_t id, std::array<float, 3>& outPos) const;


	private:
		ImVec2 m_LastSize = { 0, 0 };   // 最後に記憶したサイズ
		ImVec2 m_PanOffset = { 0, 0 };  // パン（ドラッグ移動）用
		std::vector<SceneSprite> m_SceneSprites; // シーンに配置されたスプライトのリスト
		std::vector<std::string> m_PendingDrop; // ドロップ待ちのアセット名リスト
		std::vector<ImVec2> m_PendingDropPos; // ドロップ待ちのアセット位置リスト
		//std::unordered_map<int64_t, SceneSprite> sprites;
	};

	window_scene& instance_winSce();
}

#endif // !WINDOW_SCENE