#include "include/window_editor/window_inspector.h"
#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/window_scene.h"
#include "include/window_editor/hierarchy/hierarchy_model.h"
#include "include/component/component_config.hpp"
#include "include/component/component_keycapture.h"

namespace n_windowinspector
{
    void window_inspector::SetWindowManager(n_windowmanager::window_manager* wm)
    {
        windowManager_ = wm;
	}

	void window_inspector::Render()
	{
        // Inspector UI の描画処理をここに実装
        ImGui::SetNextWindowPos(ImVec2(450, 600), ImGuiCond_Always);

        ImGui::Begin("Hierarchy Inspector");

        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(470, 400));

        EntityId selected = NOT_SELECTED;

		//printf("[Inspector] windowManager_=%p selected=%lld \n", (void*)windowManager_, (long long)selected);

        // 選択中のエンティティIDを取得
        if (windowManager_)
        {
            if (auto* scene = windowManager_->GetSceneWindow())
            {
				//printf("[Inspector] got scene window %p\n", (void*)scene);
                selected = scene->GetSelectedEntity();
				//printf("[Inspector] selected entity id=%lld\n", (long long)selected);
            }
        }

        if (selected != NOT_SELECTED) {

            // コンポーネント用の Inspector を直接呼ぶ
            ImGui::Text("----Transform----");
            RenderTransformInspector(selected);

            ImGui::Text("----Move----");
            RenderMoveInspector(selected);

            ImGui::Text("----Rigidbody----");
            RenderRigidbodyInspector(selected);

            ImGui::Text("----IsPlayer----");
            RenderIsPlayerInspector(selected);

            ImGui::Text("----Start----");
            RenderStartInspector(selected);

            ImGui::Text("----Finish----");
            RenderFinishInspector(selected);

            ImGui::Text("----Light----");
            RenderLightInspector(selected);
            // キーキャプチャモーダルは毎フレームどこかで必ず呼ぶ（UI ループ内）
            RenderKeyCaptureModal();
        }
        else
        {
            ImGui::Text("No selection");
        }

        ImGui::End();
	}
}