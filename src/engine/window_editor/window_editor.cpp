// 基底クラス

#include "../../include/window_editor/window_editor.hpp"
#include "../../include/render.hpp"
#include "../../include/window.hpp"

#include <Windows.h>


namespace engine::editor
{

    window_editor::window_editor(const std::string& title)
        : m_Title(title),
          m_IsOpen(true),
          m_Flags(ImGuiWindowFlags_None),
          m_WindowSize(400, 300),               // ← デフォルト 400×300
          m_InitCond(ImGuiCond_FirstUseEver)// ← デフォルト挙動
    {
    }

    void window_editor::window_manager_other_Render()
    {
        // ウィンドウが閉じられていれば描画しない
        if (!m_IsOpen)
            return;

        // 初回サイズを設定
        ImGui::SetNextWindowSize(m_WindowSize, m_InitCond);

        // Begin -> OnImGuiRender -> End の流れを共通化
        ImGui::Begin(m_Title.c_str(), &m_IsOpen, m_Flags);
        OnImGuiRender();
        ImGui::End();
    }

    bool window_editor::window_manager_other_IsVisible() const
    {
        return m_IsOpen;
    }
}