/*各editorウィンドウの生成・削除を一元管理*/

#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#pragma once
#include <vector>
#include <memory>
#include "window_scene.hpp"

namespace n_windowmanager
{
    class window_manager
    {
    public:

        void RegisterSceneWindow()
        {
            windows_.emplace_back(std::make_unique<n_windowscene::window_scene>());
        }

        void RenderAll()
        {
            for (auto& window : windows_)
            {
                window->window_scene_Render();
            }
        }

    private:
        std::vector<std::unique_ptr<n_windowscene::window_scene>> windows_;
    };
}

#endif // !WINDOW_MANAGER