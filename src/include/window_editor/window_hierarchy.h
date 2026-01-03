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



/*

    Hierarchy 責務分離メモ
    1. UI（n_hierarchy::ui）

    役割
    ImGui を使った描画・ユーザー操作を担当
    hierarchyModel からオブジェクトリストを取得して表示
    選択・クリックイベントを検出して Sync に通知
    描画以外のロジックや状態は持たない

    やること
    DrawHierarchy(hierarchyModel&)
    ImGui::Selectable で選択を検知
    クリックされた EntityId を sync::OnEntitySelected() に送信

    やらないこと
    EntityId の決定
    選択状態の保持
    Scene や Component の直接操作

    2. Sync（n_hierarchy::sync）

    役割
    UI と Scene / Model の橋渡し
    ユーザー操作や Scene からのイベントを受け取り、正しい場所に通知
    Scene の唯一の選択状態 (selectedEntity_) を更新する

    やること
    OnEntitySelected(EntityId eid) → Scene に通知 (SetSelectedEntity)
    OnSceneObjectAdded/Removed(SceneToHierarchyObj) → hierarchyModel に通知
    EntityId を Scene / Model 間で伝搬する

    やらないこと
    選択状態を自分で保持しない
    ImGui 描画

    3. Model（n_hierarchy::hierarchyModel）

    役割
    表示用データの保持
    Scene から通知された状態をキャッシュして UI に提供
    スレッドセーフなオブジェクトリスト管理

    やること
    オブジェクト追加・削除 (AddObject/RemoveObject)
    キャッシュされた EntityId / SpriteId の保持
    選択フラグの保持（あくまで UI 描画用）

    やらないこと
    EntityId の生成
    Scene 上の本当の選択状態を決定
    Scene / Component に直接変更を加える

*/



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
