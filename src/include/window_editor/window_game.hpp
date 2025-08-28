#ifndef WINDOW_GAME
#define WINDOW_GAME

#pragma once
#include <imgui.h>
#include <memory>
#include "window_base.hpp"

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

#endif // !WINDOW_GAME
