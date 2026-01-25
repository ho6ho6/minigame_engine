#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/hierarchy/hierarchy_ui.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"
#include "include/window_editor/window_manager.h"


namespace n_windowhierarchy
{

    void window_hierarchy::Render()
    {
        //printf("[WH/Render] this=%p, &hierarchyModel_=%p, sizeof(hierarchyModel)=%zu\n", 
        //    (void*)this, (void*)&hierarchyModel_, sizeof(n_hierarchy::hierarchyModel));
        if (!wm_) return;
        
        n_hierarchy::ui::DrawHierarchy(hierarchyModel_, wm_);
	}

    void window_hierarchy::SetWindowManager(n_windowmanager::window_manager* wm)
    {
        wm_ = wm;
    }
    
	// エンティティIDからスプライトIDを取得
    std::optional<EntityId> window_hierarchy::GetSpriteIdForEntity(EntityId eid) const
    {
		return n_hierarchy::sync::GetSpriteForEntity(eid);
    }

}