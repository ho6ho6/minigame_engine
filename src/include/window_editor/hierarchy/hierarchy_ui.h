#ifndef HIERARCHY_UI_H
#define HIERARCHY_UI_H

#include "include/window_editor/hierarchy/hierarchy_model.h"
#include "include/window_editor/window_manager.h"

namespace n_hierarchy::ui
{
	void DrawHierarchy(hierarchyModel& model, n_windowmanager::window_manager* wm);
}

#endif // !HIERARCHY_UI_H
