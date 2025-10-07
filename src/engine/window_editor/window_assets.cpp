#include "../../include/window_editor/window_assets.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11
#include <imgui.h>

/*Assetsウィンドウの描画*/
namespace n_windowassets
{
	void window_assets::Render()
	{
        /*windowの座標とサイズ*/
        ImGui::SetNextWindowPos(ImVec2(400, 600), ImGuiCond_Always);

        //ImGui::SetNextWindowSizeConstraints(ImVec2(600, 400), ImVec2(600, 400));


        // Window表示に関して
        ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

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

        ImGui::End();
	}
}