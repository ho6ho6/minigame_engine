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

			// window_editorのインタフェース
		protected:
			void OnImGuiRender() override;

		private:
			// ビューポート用テクスチャID or フレームバッファ識別子
			void* window_scene_ViewportTexture = nullptr;

			// ウィンドウ内サイズキャッシュ
			ImVec2 window_scene_LastSize = { 0,0 };
	};
}

#endif // !WINDOW_SCENE
