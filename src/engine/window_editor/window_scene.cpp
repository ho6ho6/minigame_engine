/*シーンウィンドウ*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11
#include "imgui_impl_win32.h"		//ImGuiでWin32API
#include "imgui.h"					//ImGui本体
#include <math.h>

namespace n_windowscene
{

    void window_scene::Render()
    {
        /*windowの座標とサイズ*/
        ImGui::SetNextWindowPos(ImVec2(400, 0), ImGuiCond_Always);

        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 600), ImVec2(600, 600));


        // Window表示に関して
        ImGui::Begin("Scene_View", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x != m_LastSize.x || avail.y != m_LastSize.y) {
            m_LastSize = avail;
            n_render::Render_Resizeviewport((int)avail.x, (int)avail.y);
        }

        ImTextureID texID = reinterpret_cast<ImTextureID>(n_render::Render_GetSceneSRV());
        ImGui::Image(texID, m_LastSize);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto mp = ImGui::GetMousePos();
            n_render::Render_PickObject((int)mp.x, (int)mp.y);
        }

        ImGui::Image((void*)n_render::Render_GetSceneSRV(), avail);

        /*ここまで*/


        /*ここから座標系の描画*/

        // 1) 描画用リストを取得
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // 2) ウィンドウ左上スクリーン座標 & 利用可能領域サイズ
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        const ImVec2 contentOffset = ImGui::GetCursorScreenPos();
        // if you used Image, cursor has moved—`GetWindowPos()` + TitleBarHeight で代替可

      // 3) 座標系の中心を「利用領域中央」に設定
        const float centerX = winPos.x + winSize.x * 0.5f;
        const float centerY = winPos.y + winSize.y * 0.5f;
        const ImVec2 origin(centerX, centerY);

        // 4) グリッド描画（10px 間隔／薄い線）
        const float gridStep = 20.0f;
        const ImU32  colGrid = IM_COL32(200, 200, 200, 50);
        // 垂直線
        for (float x = fmodf(origin.x, gridStep); x < origin.x + winSize.x; x += gridStep)
            draw->AddLine(
                ImVec2(x, origin.y - winSize.y * 0.5f),
                ImVec2(x, origin.y + winSize.y * 0.5f),
                colGrid);
        // 水平線
        for (float y = fmodf(origin.y, gridStep); y < origin.y + winSize.y; y += gridStep)
            draw->AddLine(
                ImVec2(origin.x - winSize.x * 0.5f, y),
                ImVec2(origin.x + winSize.x * 0.5f, y),
                colGrid);

        // 5) X/Y 軸描画（中央の太線）
        const ImU32 colAxisX = IM_COL32(255, 0, 0, 255);
        draw->AddLine(
            ImVec2(origin.x - winSize.x * 0.5f, origin.y),
            ImVec2(origin.x + winSize.x * 0.5f, origin.y),
            colAxisX, 2.0f);
        const ImU32 colAxisY = IM_COL32(0, 255, 0, 255);
        draw->AddLine(
            ImVec2(origin.x, origin.y - winSize.y * 0.5f),
            ImVec2(origin.x, origin.y + winSize.y * 0.5f),
            colAxisY, 2.0f);

        // 6) 原点ラベル
        const ImU32 colText = IM_COL32(255, 255, 255, 200);
        draw->AddText(origin, colText, "(0,0)");

        /*ここから座標系の描画*/
        
        

        ImGui::End();
    }
}