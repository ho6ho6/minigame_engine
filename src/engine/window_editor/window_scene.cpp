/*シーンウィンドウ*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11

namespace n_windowscene
{
    void window_scene::window_scene_Render()
    {
        ImGui::Begin("Scene View");

        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x != m_LastSize.x || avail.y != m_LastSize.y) {
            m_LastSize = avail;
            render::Render_Resizeview((int)avail.x, (int)avail.y);
        }

        ImTextureID texID = reinterpret_cast<ImTextureID>(render::Render_GetSceneSRV());
        ImGui::Image(texID, m_LastSize);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto mp = ImGui::GetMousePos();
            render::Render_PickObject((int)mp.x, (int)mp.y);
        }

        ImGui::End();
    }
}