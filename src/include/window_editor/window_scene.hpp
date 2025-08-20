#ifndef WINDOW_SCENE
#define WINDOW_SCENE

#pragma once
#include <imgui.h>
#include <memory>

namespace n_windowscene
{
    class window_scene
    {
    public:
        window_scene() = default;

        void window_scene_Render(); // –ˆƒtƒŒ[ƒ€ŒÄ‚Ño‚·•`‰æŠÖ”

    private:
        ImVec2 m_LastSize = { 0, 0 };
    };
}

#endif // !WINDOW_SCENE
