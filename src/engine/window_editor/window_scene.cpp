/*シーンウィンドウ*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui.h"					//ImGui本体

static ImVec2 ViewOffset = ImVec2(0.0f, 0.0f);

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

        //パン処理
		ImGuiIO& io = ImGui::GetIO();

        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::Text("MousePos=(%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        ImGui::InvisibleButton("##window_scene", avail, ImGuiButtonFlags_MouseButtonRight);

        //  if(io.MouseDown[ImGuiMouseButton_Right]) {
		//	ImGui::SetCursorPos(ImVec2(10, 50));
        //  ImGui::Text("Right_True");
		//}

        if (io.MouseDown[ImGuiMouseButton_Right]) {
            ViewOffset.x += io.MouseDelta.x;
            ViewOffset.y += io.MouseDelta.y;
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
		}

        ImGui::Image((void*)n_render::Render_GetSceneSRV(), avail);

        /*ここまで*/


        /*ここから座標系の描画*/


        /**********グリッド描画の初期設定**********/

        // 1) 描画用リストを取得
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // 2) ウィンドウ左上スクリーン座標 & 利用可能領域サイズ
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        const ImVec2 contentOffset = ImGui::GetCursorScreenPos();
        
		draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);   //ウィンドウ外に描画しないようにクリップ


        // 3) 座標系の中心を「利用領域中央」に設定
        float centerX = winPos.x + winSize.x * 0.5f + ViewOffset.x;
        float centerY = winPos.y + winSize.y * 0.5f + ViewOffset.y;
        ImVec2 origin(centerX, centerY);

        // 4) グリッド描画（10px 間隔／薄い線）
        const float gridStep = 20.0f;
        const ImU32  colGrid = IM_COL32(200, 200, 200, 50);


        /**********グリッド描画の初期設定**********/

        // 垂直線
        for (float x = fmodf(origin.x, gridStep); x < (centerX + winSize.x) * 100; x += gridStep)
            draw->AddLine(
                ImVec2(x, origin.y - winSize.y * 0.5f),
                ImVec2(x, origin.y + winSize.y * 0.5f),
                colGrid);
        // 水平線
        for (float y = fmodf(origin.y, gridStep); y < (centerY + winSize.y) * 100; y += gridStep)
            draw->AddLine(
                ImVec2(origin.x - winSize.x * 0.5f, y),
                ImVec2(origin.x + winSize.x * 0.5f, y),
                colGrid);

        // 5) X/Y 軸描画（中央の太線）
        const ImU32 colAxisX = IM_COL32(255, 0, 0, 255);
        draw->AddLine(
            ImVec2(origin.x - winSize.x * 10.0f, origin.y),
            ImVec2(origin.x + winSize.x * 10.0f, origin.y),
            colAxisX, 2.0f);
        const ImU32 colAxisY = IM_COL32(0, 255, 0, 255);
        draw->AddLine(
            ImVec2(origin.x, origin.y - winSize.y * 10.0f),
            ImVec2(origin.x, origin.y + winSize.y * 10.0f),
            colAxisY, 2.0f);

        // 6) 原点ラベル
        const ImU32 colText = IM_COL32(255, 255, 255, 200);
        draw->AddText(origin, colText, "(0,0)");

        /*ここから座標系の描画*/
        
        

        ImGui::End();
    }
}