// ���N���X

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
          m_WindowSize(400, 300),               // �� �f�t�H���g 400�~300
          m_InitCond(ImGuiCond_FirstUseEver)// �� �f�t�H���g����
    {
    }

    void window_editor::window_manager_other_Render()
    {
        // �E�B���h�E�������Ă���Ε`�悵�Ȃ�
        if (!m_IsOpen)
            return;

        // ����T�C�Y��ݒ�
        ImGui::SetNextWindowSize(m_WindowSize, m_InitCond);

        // Begin -> OnImGuiRender -> End �̗�������ʉ�
        ImGui::Begin(m_Title.c_str(), &m_IsOpen, m_Flags);
        OnImGuiRender();
        ImGui::End();
    }

    bool window_editor::window_manager_other_IsVisible() const
    {
        return m_IsOpen;
    }
}