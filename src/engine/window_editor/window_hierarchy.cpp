#include "include/window_editor/window_hierarchy.hpp"
#include "include/render.hpp"	//フレームバッファ取得用
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11

namespace n_windowhierarchy
{

    void window_hierarchy::Render()
    {
        /*windowの座標とサイズ*/
        ImGui::SetNextWindowPos(ImVec2(0, 600), ImGuiCond_Always);

        //ImGui::SetNextWindowSizeConstraints(ImVec2(400, 435), ImVec2(400, 480));

        // Window表示に関して
        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize);

        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(920, 400));

        ImVec2 avail = ImGui::GetContentRegionAvail();

        // サイズ変更の検出　安全策として 0 以下は無視
        if (avail.x <= 0.0f || avail.y <= 0.0f)
        {
            if (m_LastSize.x <= 0.0f || m_LastSize.y <= 0.0f) m_LastSize = ImVec2(1.0f, 1.0f);
        }
        else if (avail.x != m_LastSize.x || avail.y != m_LastSize.y)
        {
            m_LastSize = avail;
        }

        ImGuiIO& io = ImGui::GetIO();

        // 論理サイズとフレームバッファサイズ
        int logical_w = (int)avail.x;
        int logical_h = (int)avail.y;
        int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
        int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

        n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);

        ImTextureID texID = reinterpret_cast<ImTextureID>(n_render::Render_GetSceneSRV());
        ImGui::Image(texID, m_LastSize);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto mp = ImGui::GetMousePos();
            n_render::Render_PickObject((int)mp.x, (int)mp.y);
        }

        ImGui::Image((void*)n_render::Render_GetSceneSRV(), avail);

        ImGui::End();
    }
}