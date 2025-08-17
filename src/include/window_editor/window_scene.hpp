#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include "window_editor.hpp"
#include <imgui.h>
#include <memory>

namespace engine::editor
{
	class window_scene : public window_editor
	{
		public:
			window_scene();

			// window_editor�̃C���^�t�F�[�X
		protected:
			void OnImGuiRender() override;

		private:
			// �r���[�|�[�g�p�e�N�X�`��ID or �t���[���o�b�t�@���ʎq
			void* window_scene_ViewportTexture = nullptr;

			// �E�B���h�E���T�C�Y�L���b�V��
			ImVec2 window_scene_LastSize = { 0,0 };
	};
}

#endif // !WINDOW_SCENE
