/*各editorウィンドウの生成・削除を一元管理*/

#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#pragma once
#include <vector>
#include <memory>
#include "window_base.h"

namespace n_windowmanager
{
    class window_manager
    {
    public:
        void Register_SceneWindow();
        void Register_GameWindow();
        //void Register_InputWindow();
        void Register_Hierarchywindow();
        void Register_Assetswindow();
        void RenderAll();

    private:
        std::vector<std::unique_ptr<n_windowbase::window_base>> windows_;
    };
}

#endif // !WINDOW_MANAGER