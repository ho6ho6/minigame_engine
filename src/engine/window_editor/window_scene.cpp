/*�V�[���E�B���h�E*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/window_editor/window_editor.hpp"
#include "../../include/render.hpp"	//�t���[���o�b�t�@�擾�p
#include "imgui_impl_dx11.h"		//ImGui��DirectX11

#include <Windows.h>
#include "../../include/window.hpp"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::editor
{
	window_scene::window_scene() : window_editor("Scene View")
	{
        window_editor_SetInitialSize({ 800, 600 });
	}

    void window_scene::OnImGuiRender()
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x != window_scene_LastSize.x || avail.y != window_scene_LastSize.y) {
            window_scene_LastSize = avail;
            render::Render_Resizeview((int)avail.x, (int)avail.y);
        }

        // ���t���[�������ōŐV��SRV���擾����ImGui�ɓn��
        ImTextureID texID = reinterpret_cast<ImTextureID>(render::Render_GetSceneSRV());
        ImGui::Image(texID, window_scene_LastSize);


        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto mp = ImGui::GetMousePos();
            render::Render_PickObject((int)mp.x, (int)mp.y);
        }
    }
}