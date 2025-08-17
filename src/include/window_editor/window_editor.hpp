/*�eEditor�E�B���h�E�̋��ʒ��ۃN���X*/

#ifndef WINDOW_EDITOR
#define WINDOW_EDITOR

#pragma once
#include <string>
#include "imgui.h" // ImGui�̃w�b�_�[�t�@�C��
#include "window_manager_abstra.hpp"
#include "window_manager_other.hpp" // window_manager_other�̃C���N���[�h

#ifdef _WIN32
#include <Windows.h> // HINSTANCE, HWND, WNDCLASSEX, ...
#endif

namespace engine::editor
{
	class window_editor : public engine::window::window_manager_other
	{
        public:
            // �錾�̂݁B�����ɖ{�̂������Ȃ�
            explicit window_editor(const std::string& title);
            virtual ~window_editor() = default;

            void window_editor_SetInitialSize(const ImVec2& size, ImGuiCond cond = ImGuiCond_FirstUseEver)
            {
                m_WindowSize = size;
                m_InitCond = cond;
			}

            // �C�x���g���[�v����Ăяo��
			void window_manager_other_Render() override; // ���t���[���Ăяo�����`��֐� ImGui ::Begin -> OnImGuiRender -> ImGui::End �̗�������ʉ�
            bool window_manager_other_IsVisible() const override; // �\���^��\���Ǘ� 

            void window_editor_CreateWindow(HINSTANCE hInst, int x, int y, int width, int height);

        protected:
            // ���ۂ̒��g���������z���\�b�h
            virtual void OnImGuiRender() = 0;
            
            bool                    m_IsOpen    = true;                              // �J���Ă��邩
            std::string               m_Title;                                // �E�B���h�E�^�C�g���ۑ��p
            ImGuiWindowFlags     m_Flags      = ImGuiWindowFlags_NoCollapse;  // �t���O������
			ImVec2                m_WindowSize;               // �����E�B���h�E�T�C�Y
			ImGuiCond			    m_InitCond; // ����g�p���̏���
    };
}

#endif // !WINDOW_EDITOR
