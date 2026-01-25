#ifndef SCENE_UI_H
#define SCENE_UI_H

#include "include/window_editor/scene/scene_ctx.h"

#include <imgui.h>

namespace n_windowscene::ui
{
	// ui表示
	void SetupScene();

	// Sceneに描画されるグリッド描画
	void DrawSceneGrid(const SceneViewContext& ctx, ImDrawList* draw);

	// window_sceneの当たり判定デバック
	void DebugSceneFrame(const SceneViewContext& ctx, ImDrawList* draw);
}

#endif // !SCENE_UI_H