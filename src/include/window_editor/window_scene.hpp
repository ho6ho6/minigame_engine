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
            static window_scene& Get();
			void AddAssetToScene(Texture* tex, const std::string& asset_name, ImVec2 guiLocalPos, ImVec2 guiWindowPos);

        private:
			ImVec2 m_LastSize = { 0, 0 };   // �Ō�Ɋm�肵���T�C�Y
			ImVec2 m_PanOffset = { 0, 0 };  // �p���i�h���b�O�ړ��j��
			std::vector<SceneSprite> m_SceneSprites; // �V�[���ɔz�u���ꂽ�X�v���C�g�̃��X�g
			std::vector<std::string> m_PendingDrop; // �h���b�v�҂��̃A�Z�b�g�����X�g
			std::vector<ImVec2> m_PendingDropPos; // �h���b�v�҂��̃A�Z�b�g�ʒu���X�g
    };
}

#endif // !WINDOW_SCENE
