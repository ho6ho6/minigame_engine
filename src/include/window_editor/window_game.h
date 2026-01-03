#ifndef WINDOW_GAME_H
#define WINDOW_GAME_H

#include <imgui.h>
#include "window_base.h"

namespace n_windowgame
{
    class window_game : public n_windowbase::window_base
    {
    public:
        void Render() override;
    private:
        ImVec2 m_LastSize = { 0, 0 };
    };
}

#endif // !WINDOW_GAME_H