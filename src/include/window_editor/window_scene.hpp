#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include <imgui.h>
#include "window_base.hpp"
#include "window_scene_sprite.hpp"
#include <vector>
#include <iostream>

namespace n_windowscene
{
    class window_scene : public n_windowbase::window_base
    {
        public:
            void Render() override;
			void AddAssetToScene(Texture* tex, const std::string& asset_name, ImVec2 guiLocalPos, ImVec2 guiWindowPos);

        private:
			ImVec2 m_LastSize = { 0, 0 };   // 最後に確定したサイズ
			ImVec2 m_PanOffset = { 0, 0 };  // パン（ドラッグ移動）量
			std::vector<SceneSprite> m_SceneSprites; // シーンに配置されたスプライトのリスト
			std::vector<std::string> m_PendingDrop; // ドロップ待ちのアセット名リスト
			std::vector<ImVec2> m_PendingDropPos; // ドロップ待ちのアセット位置リスト
    };
}

#endif // !WINDOW_SCENE
