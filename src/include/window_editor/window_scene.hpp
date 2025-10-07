#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include <imgui.h>
#include "window_base.hpp"

namespace n_windowscene
{
    class window_scene : public n_windowbase::window_base
    {
        public:
            void Render() override;
        private:
			ImVec2 m_LastSize = { 0, 0 };   // 最後に確定したサイズ
			ImVec2 m_PanOffset = { 0, 0 };  // パン（ドラッグ移動）量
    };
}

#endif // !WINDOW_SCENE
