/*各Editorウィンドウの共通抽象クラス*/

#ifndef WINDOW_EDITOR
#define WINDOW_EDITOR

#pragma once
#include <string>
#include "imgui.h" // ImGuiのヘッダーファイル
#include "window_manager_abstra.hpp"
#include "window_manager_other.hpp" // window_manager_otherのインクルード

#ifdef _WIN32
#include <Windows.h> // HINSTANCE, HWND, WNDCLASSEX, ...
#endif

namespace engine::editor
{
	class window_editor : public engine::window::window_manager_other
	{
        public:
            // 宣言のみ。ここに本体を書かない
            explicit window_editor(const std::string& title);
            virtual ~window_editor() = default;

            void window_editor_SetInitialSize(const ImVec2& size, ImGuiCond cond = ImGuiCond_FirstUseEver)
            {
                m_WindowSize = size;
                m_InitCond = cond;
			}

            // イベントループから呼び出す
			void window_manager_other_Render() override; // 毎フレーム呼び出される描画関数 ImGui ::Begin -> OnImGuiRender -> ImGui::End の流れを共通化
            bool window_manager_other_IsVisible() const override; // 表示／非表示管理 

            void window_editor_CreateWindow(HINSTANCE hInst, int x, int y, int width, int height);

        protected:
            // 実際の中身を書く仮想メソッド
            virtual void OnImGuiRender() = 0;
            
            bool                    m_IsOpen    = true;                              // 開いてあるか
            std::string               m_Title;                                // ウィンドウタイトル保存用
            ImGuiWindowFlags     m_Flags      = ImGuiWindowFlags_NoCollapse;  // フラグ初期化
			ImVec2                m_WindowSize;               // 初期ウィンドウサイズ
			ImGuiCond			    m_InitCond; // 初回使用時の条件
    };
}

#endif // !WINDOW_EDITOR
