/*�V�[���E�B���h�E*/
#include "../../include/window_editor/window_game.hpp"
#include "../../include/render.hpp"	//�t���[���o�b�t�@�擾�p
#include "imgui_impl_dx11.h"		//ImGui��DirectX11

namespace n_windowgame
{

    void window_game::Render()
    {
        /*window�̍��W�ƃT�C�Y*/
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

        //ImGui::SetNextWindowSizeConstraints(ImVec2(400, 200), ImVec2(FLT_MAX, FLT_MAX));


        // Window�\���Ɋւ���
        ImGui::Begin("Game_View");

        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x != m_LastSize.x || avail.y != m_LastSize.y) {
            m_LastSize = avail;
        }

        // �_���T�C�Y�ƃt���[���o�b�t�@�T�C�Y
        if (avail.x > 1.0f && avail.y > 1.0f) {
            int logical_w = (int)avail.x;
            int logical_h = (int)avail.y;
            ImGuiIO& io = ImGui::GetIO();
            int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
            int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

            if (logical_w != m_LastSize.x || logical_h != m_LastSize.y) {
                m_LastSize = ImVec2((float)logical_w, (float)logical_h);
                n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);
            }
        }

        ImTextureID texID = reinterpret_cast<ImTextureID>(n_render::Render_GetSceneSRV());
        ImGui::Image(texID, m_LastSize);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto mp = ImGui::GetMousePos();
            n_render::Render_PickObject((int)mp.x, (int)mp.y);
        }

        ImGui::Image((void*)n_render::Render_GetSceneSRV(), avail);


        /*window_scene�̉�ʂ����W�n���폜���āA���̂܂܃Q�[����ʂƂ��ē��e����*/

        ImGui::End();
    }
}