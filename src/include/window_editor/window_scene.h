/*実際にアセット配置を行う*/

#ifndef WINDOW_SCENE_H
#define WINDOW_SCENE_H

#define NOT_SELECTED -1

#include <imgui.h>
#include "window_base.h"
#include "window_scene_sprite.h"
#include "scene/scene_input.h"
#include "scene/scene_ctx.h"
#include "include/component/componentDefaults.h"
#include "include/window_editor/window_manager.h"
#include <vector>
#include <iostream>
#include <array>
#include <optional>

namespace n_windowscene
{
	class window_scene : public n_windowbase::window_base
	{
		public:
			void Render() override;
			void AddAssetToScene(Texture* tex, const std::string& asset_name, ImVec2 guiLocalPos, ImVec2 guiWindowPos, const SceneViewContext& ctx);
			EntityId GenerateUniqueSpriteId();	// ユニークID生成 <-　これはスプライト自身のID

			/*　int64_t は基本的にCompoentが付与されるオブジェクトのID　*/
			bool GetSpritePosition(EntityId id, std::array<float, 2>& outPos) const;
			bool SetSpritePosition(EntityId id, const std::array<float, 2>& SetPos);
			void RegisterSprite(EntityId id, const SceneSprite& sprite);

			/* 新しいwindow_scene関数 */
			// 各オブジェクトに固有のIDを付与
			void SetSelectedEntity(EntityId eid);
			EntityId GetSelectedEntity() const;

			void SetWindowManager(n_windowmanager::window_manager* wm) { windowManager_ = wm; }
			/* 新しいwindow_scene関数 */

		private:
			ImVec2 m_LastSize  =	 { 0, 0 };			// 最後に記憶したサイズ
			ImVec2 m_PanOffset =	 { 0, 0 };			// パン（ドラッグ移動）用
			std::vector<SceneSprite> m_SceneSprites;	// シーンに配置されたスプライトのリスト
			std::vector<std::string> m_PendingDrop;		// ドロップ待ちのアセット名リスト
			std::vector<ImVec2>		 m_PendingDropPos;	// ドロップ待ちのアセット位置リスト
			SceneViewCamera			 m_camera;// sceneを映すカメラ(ゲームのカメラと統一する可能性もある)

			/* 新しいwindow_scene関数 */
			n_windowmanager::window_manager* windowManager_ = nullptr;
			EntityId selectedEntity_ = NOT_SELECTED;
			EntityId g_NextSpriteId = 0; // ユニークID生成用カウンタ
			/* 新しいwindow_scene関数 */
	};

	window_scene& instance_winSce();
}

#endif // !WINDOW_SCENE_H