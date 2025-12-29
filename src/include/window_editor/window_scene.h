#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include <imgui.h>
#include "window_base.h"
#include "window_scene_sprite.h"
#include <vector>
#include <iostream>
#include <array>
#include <optional>

namespace n_windowscene
{
	class window_scene : public n_windowbase::window_base
	{
	public:
		void Render() override;
		void AddAssetToScene(Texture* tex, const std::string& asset_name, ImVec2 guiLocalPos, ImVec2 guiWindowPos, const ImVec2& ContentSize);
		void DeleteAssetFromScene(uint64_t id);
		uint64_t GenerateUniqueSpriteId();	// ユニークID生成 <-　これはスプライト自身のID

		/*　int64_t は基本的にCompoentが付与されるオブジェクトのID　*/
		bool GetSpritePosition(int64_t id, std::array<float, 2>& outPos) const;
		bool SetSpritePosition(int64_t id, const std::array<float, 2>& SetPos);
		void RegisterSprite(int64_t id, const SceneSprite& sprite);


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