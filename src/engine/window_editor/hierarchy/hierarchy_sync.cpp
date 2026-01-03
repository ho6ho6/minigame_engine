#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/window_manager.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"
#include "include/component/component_api.h"

namespace n_hierarchy::sync
{
	std::mutex hiesyncMtx;
	std::unordered_map<EntityId, EntityId> entityToSprite;

	// 初期化（Editor 起動時に一度だけ呼ぶ）
	static n_windowmanager::window_manager* g_windowManager = nullptr;

	// mutex とマップは同期用に使用
	void Initialize(n_windowmanager::window_manager* wm)
	{
		g_windowManager = wm;
	}

	/* ---Sceneにオブジェクトが追加 / 削除されたら 通知される--- */

	void OnSceneObjectAdded(const SceneToHierarchyObj& obj)
	{
		printf("[HierarchySync] AddObject id=%lld name=%s\n",
			(long long)obj.id, obj.name.c_str());

		if (!g_windowManager) 
		{ 
			printf("[HierarchySync] no windowManager\n"); 
			return; 
		}

		auto* hierarchy = g_windowManager->GetHierarchyWindow();
		if (!hierarchy) 
		{ 
			printf("[HierarchySync] no hierarchy window\n"); 
			return; 
		}
		
		//printf("[HierarchySync] hierarchy ptr=%p\n", (void*)hierarchy);
		
		hierarchy->GetHierarchyModel().AddObject(obj);
	}

	void OnSceneObjectRemoved(EntityId eid)
	{
		if (!g_windowManager) return;

		auto* hierarchy = g_windowManager->GetHierarchyWindow();
		if (!hierarchy) return;

		hierarchy->GetHierarchyModel().RemoveObject(eid);
	}

	/* ---Sceneにオブジェクトが追加 / 削除されたら 通知される--- */



	// ヒエラルキー上でエンティティが選択された
	void OnEntitySelected(EntityId eid)
	{
		printf("[HierarchySync] OnEntitySelected eid=%lld\n",
			(long long)eid);

		if (!g_windowManager) 
		{
			printf("[HierarchySync] g_windowManager is null\n");
			return;
		}

		auto* scene = g_windowManager->GetSceneWindow();
		if (!scene)
		{
			printf("[HierarchySync] scene is null\n");
			return;
		}

		// sceneに選択状態のみを伝える
		scene->SetSelectedEntity(eid);	// 状態は保持しない・値をコピーしない
	}
	


	/* ---スプライトコンポーネントが追加 / 削除された--- */

	void OnSpriteComponentAdded(EntityId eid, const n_component::SpriteComponent& sc)
	{
		std::lock_guard lk(hiesyncMtx);
		entityToSprite[eid] = sc.spriteId;
	}

	void OnSpriteComponentRemoved(EntityId eid)
	{
		std::lock_guard lk(hiesyncMtx);
		entityToSprite.erase(eid);
	}

	/* ---スプライトコンポーネントが追加 / 削除された--- */



	// entityId から spriteId を取得（存在しない場合は std::nullopt）
	std::optional<EntityId> GetSpriteForEntity(EntityId eid)
	{
		std::lock_guard lk(hiesyncMtx);
		auto it = entityToSprite.find(eid);
		if (it == entityToSprite.end()) return std::nullopt;
		return it->second;
	}
}