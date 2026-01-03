#include "include/window_editor/hierarchy/hierarchy_ui.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"
#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_manager.h"
#include <imgui.h>

namespace n_hierarchy::ui
{
	// ヒエラルキーウィンドウの描画
    void DrawHierarchy(hierarchyModel& model, n_windowmanager::window_manager* wm)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 600), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(450, 400));
        ImGui::Begin("Hierarchy");

        auto objects = model.Snapshot();
        
        auto* scene = wm->GetSceneWindow();
        EntityId selected = scene ? scene->GetSelectedEntity() : NOT_SELECTED;

		//printf("[HierarchyUI] selected entity id=%lld\n", (long long)selected);

        //printf("[HierarchyUI] param &model=%p, sizeof(hierarchyModel)=%zu\n", 
        //    (void*)std::addressof(model), sizeof(n_hierarchy::hierarchyModel));s
        //printf("[HierarchyUI] DrawHierarchy got objects.size=%zu\n", objects.size());
        //for (size_t i = 0; i < objects.size(); ++i) 
        //{ 
        //    const auto& o = objects[i]; 
        //    printf("[HierarchyUI] obj[%zu] id=%lld name=%s registered=%d selected=%d\n", i, (long long)o.id, o.name.c_str(), (int)o.registered, (int)o.selected);
        //}

        for (const auto& obj : objects)
        {
            bool isSelected = (obj.id == selected);

            ImGui::PushID(obj.id);


            if (ImGui::Selectable(obj.name.c_str(), isSelected))
            {
                //model.SelectEntity(obj.id);
				//printf("[HierarchyUI] Selectable clicked id=%lld name=%s\n", (long long)obj.id, obj.name.c_str());
                sync::OnEntitySelected(obj.id);
            }

            ImGui::PopID();

        }

        ImGui::End();
    }
}