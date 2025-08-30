#include "../../include/window_editor/window_hierarchy.hpp"
#include "../../include/render.hpp"	//�t���[���o�b�t�@�擾�p
#include "imgui_impl_dx11.h"		//ImGui��DirectX11

namespace n_windowhierarchy
{

    void window_hierarchy::Render()
    {
        /*window�̍��W�ƃT�C�Y*/
        ImGui::SetNextWindowPos(ImVec2(0, 568), ImGuiCond_Always);

        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 435), ImVec2(400, 435));


        // Window�\���Ɋւ���
        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

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