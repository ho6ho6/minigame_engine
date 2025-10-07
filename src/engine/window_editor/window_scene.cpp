/*シーンウィンドウ*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui.h"					//ImGui本体
#include <stdio.h>

static ImVec2 ViewOffset = ImVec2(0.0f, 0.0f);

namespace n_windowscene
{

    void window_scene::Render()
    {
        ImGui::SetNextWindowPos(ImVec2(400, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 600), ImVec2(600, 600));

        ImGui::Begin("Scene_View", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

		ImVec2 avail = ImGui::GetContentRegionAvail();  // 利用可能な領域を取得

        if (avail.x != m_LastSize.x || avail.y != m_LastSize.y) {
            m_LastSize = avail;
            n_render::Render_Resizeviewport((int)avail.x, (int)avail.y);
        }

        // ヒット領域を先に作る（描画と同じサイズ・位置にするため cursor を取得）
        ImVec2 contentPos = ImGui::GetCursorScreenPos(); // 領域のスクリーン左上
        ImVec2 contentSize = avail;                       // content のサイズ（ヒット領域として使用）
        ImGui::InvisibleButton("Scene_drag_area", contentSize, ImGuiButtonFlags_MouseButtonRight);

        bool isRightActive = ImGui::IsItemActive();               // 押されている間 true
        bool isRightHovered = ImGui::IsItemHovered();             // ホバー中かどうか

        if (isRightActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right); // 累積 delta
             // 1フレーム分に変換して反映する
            ViewOffset.x += drag.x;
            ViewOffset.y += drag.y;
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測
        }

        // シーンテクスチャをヒット領域と同じ場所に表示する
        ImGui::SetCursorScreenPos(contentPos);

        // 描画はヒット領域と同じ位置へ
        ImGui::SetCursorScreenPos(contentPos);
        ImTextureID texID = reinterpret_cast<ImTextureID>(n_render::Render_GetSceneSRV());
        ImGui::Image(texID, contentSize);

        // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);

        float centerX = winPos.x + winSize.x * 0.5f + ViewOffset.x;
        float centerY = winPos.y + winSize.y * 0.5f + ViewOffset.y;
        ImVec2 origin(centerX, centerY);

        const float gridStep = 20.0f;
        const ImU32 colGrid = IM_COL32(200, 200, 200, 50);

        for (float x = fmodf(origin.x, gridStep); x < winPos.x + winSize.x; x += gridStep)
            draw->AddLine(ImVec2(x, origin.y - winSize.y * 0.5f), ImVec2(x, origin.y + winSize.y * 0.5f), colGrid);
        for (float y = fmodf(origin.y, gridStep); y < winPos.y + winSize.y; y += gridStep)
            draw->AddLine(ImVec2(origin.x - winSize.x * 0.5f, y), ImVec2(origin.x + winSize.x * 0.5f, y), colGrid);

        const ImU32 colAxisX = IM_COL32(255, 0, 0, 255);
        draw->AddLine(ImVec2(origin.x - winSize.x * 10.0f, origin.y), ImVec2(origin.x + winSize.x * 10.0f, origin.y), colAxisX, 2.0f);
        const ImU32 colAxisY = IM_COL32(0, 255, 0, 255);
        draw->AddLine(ImVec2(origin.x, origin.y - winSize.y * 10.0f), ImVec2(origin.x, origin.y + winSize.y * 10.0f), colAxisY, 2.0f);

        const ImU32 colText = IM_COL32(255, 255, 255, 200);
        draw->AddText(origin, colText, "(0,0)");

        draw->PopClipRect();

        ImGui::End();
    }
}