#ifndef HIERARCHY_SYNC_H
#define HIERARCHY_SYNC_H

#include "include/window_editor/window_scene_sprite.h"
#include "include/window_editor/window_scene.h"
#include "include/component/componentDefaults.h"
#include <optional>

namespace n_windowmanager { class window_manager; }

namespace n_hierarchy::sync
{
	// 初期化（Editor 起動時に一度だけ呼ぶ）
	void Initialize(n_windowmanager::window_manager* wm);

	// mutex とマップは同期用に使用
	extern std::mutex hiesyncMtx;
	extern std::unordered_map<EntityId, EntityId> entityToSprite;

	// Sceneにオブジェクトが追加 / 削除された ら通知される
	void OnSceneObjectAdded(const SceneToHierarchyObj& obj);
	void OnSceneObjectRemoved(EntityId eid);

	// ヒエラルキー上でエンティティが選択された
	void OnEntitySelected(EntityId eid);

	// スプライトコンポーネントが追加 / 削除された
	void OnSpriteComponentAdded(EntityId eid, const n_component::SpriteComponent& sc);
	void OnSpriteComponentRemoved(EntityId eid);

	// entityId から spriteId を取得（存在しない場合は std::nullopt）
	std::optional<EntityId> GetSpriteForEntity(EntityId entityId);
}

#endif
