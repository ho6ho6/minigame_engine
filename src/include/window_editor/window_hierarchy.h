#ifndef WINDOW_HIERARCHY_H
#define WINDOW_HIERARCHY_H

#include "window_base.h"
#include "hierarchy/hierarchy_model.h"
#include "window_manager.h"
#include "include/component/componentDefaults.h"
#include <optional>

namespace n_windowhierarchy
{
	class window_hierarchy : public n_windowbase::window_base
	{
		public:
			void Render() override;
			std::optional<EntityId> GetSpriteIdForEntity(EntityId entityId) const;
			void SetWindowManager(n_windowmanager::window_manager* wm);

			// 外部には参照だけ渡す
			n_hierarchy::hierarchyModel& GetHierarchyModel() { return hierarchyModel_; }

		private:
			// 所有元はwindow_hierarchy
			n_hierarchy::hierarchyModel hierarchyModel_;
			n_windowmanager::window_manager* wm_ = nullptr;
	};

}


#endif // !WINDOW_HIERARCHY_H

/*
#include <imgui.h>
#include "window_base.h"
#include <string>
#include <vector>
#include "window_scene_sprite.h"
#include "include/component/componentDefaults.h"
#include <functional>
#include <atomic>
#include <optional>

using Command = std::function<void(int64_t objectID)>;
using EntitySpritePair = std::pair<int64_t, int64_t>;
static std::optional<int64_t> selectedEntityId;


namespace n_windowhierarchy
{
	class window_hierarchy : public n_windowbase::window_base
	{
	public:
		void Render() override;
		void GetObjFromScene(const SceneToHierarchyObj& Obj);	// Sceneからオブジェクトを取得
		void DeleteObjFromScene(uint64_t id);	// Sceneからオブジェクト削除を取得

		// entityId と spriteId のペア
		std::vector<EntitySpritePair> GetAllVisibleEntitySpritePairs();
		// イベント駆動用: ヒエラルキーが変わったときに呼ぶ（エディタ側）
		void NotifyHierarchyChanged();
		void InitHierarchyScanner();
		void ShutdownHierarchyScanner();

		void RegisterSprite(int64_t entityId, int64_t spriteId);

		void UnregisterSprite(int64_t entityId);

		std::optional<int64_t> GetSpriteIdForEntity(int64_t entityId);

		void OnHierarchyAdd(int64_t entityId, const n_component::SpriteComponent& sc);

	private:
		ImVec2 m_LastSize = { 0,0 };

		// コンテキストメニュー用コマンド群
		std::vector<std::pair<std::string, Command>> contextMenuCommands;

	};


	// グローバルインスタンス sceneに配置されたAssetsをhierarchyに反映させるために使用
	window_hierarchy& instance_winHie();
}

*/
