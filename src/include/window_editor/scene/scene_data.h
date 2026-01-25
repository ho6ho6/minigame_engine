#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include "include/window_editor/window_scene.h"

#include <imgui.h>
#include <string>
#include <vector>

namespace n_windowscene::data
{
	// 遅延キューへ追加されたオブジェクトを登録
	void EnqueuePayloadFromAssetsWindow(SceneViewContext& ctx, std::vector<std::string>& m_PendingDrop, std::vector<ImVec2>& m_PendingDropPos);

	// sceneウィンドウで削除された時
	void DeleteAssetFromScene(EntityId id, std::vector<SceneSprite>& sprites);
}

#endif