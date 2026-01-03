#include "include/window_editor/window_manager.h"
#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_game.h"
#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/window_assets.h"
#include "include/window_editor/window_inspector.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"

namespace n_windowmanager
{


	/* ---各ウィンドウの登録と、ポインタの保存--- */

	void window_manager::Register_SceneWindow()
	{
		auto scene = std::make_unique<n_windowscene::window_scene>();
		scene_ = scene.get();			// ポインタを保存

		scene_->SetWindowManager(this);	// window_managerのポインタをsceneに渡す <- 双方向参照のために

		windows_.emplace_back(std::move(scene));	// 所有権を移動
	}

	void window_manager::Register_GameWindow()
	{
		auto game = std::make_unique<n_windowgame::window_game>();
		game_ = game.get();
		windows_.emplace_back(std::move(game));
	}

	void window_manager::Register_Hierarchywindow()
	{
		// ヒエラルキーウィンドウの生成
		auto hie = std::make_unique<n_windowhierarchy::window_hierarchy>();
		hie->SetWindowManager(this);

		hierarchy_ = hie.get();
		printf("[WM] Register_Hierarchywindow this=%p, hierarchy_=%p\n", (void*)this, (void*)hierarchy_);
		windows_.emplace_back(std::move(hie));
	}

	void window_manager::Register_Assetswindow()
	{
		auto assets = std::make_unique<n_windowassets::window_assets>();
		assets_ = assets.get();
		windows_.emplace_back(std::move(assets));
	}

	void window_manager::Register_Inspectorwindow()
	{
		auto inspector = std::make_unique<n_windowinspector::window_inspector>();
		inspector->SetWindowManager(this);
		inspector_ = inspector.get();
		windows_.emplace_back(std::move(inspector));
	}

	/* ---各ウィンドウの登録と、ポインタの保存--- */



	/* ---各ウィンドウの参照--- */

	n_windowscene::window_scene*
	window_manager::GetSceneWindow()
	{
		return scene_;
	}

	n_windowhierarchy::window_hierarchy*
	window_manager::GetHierarchyWindow()
	{
		return hierarchy_;
	}

	n_windowassets::window_assets*
	window_manager::GetAssetsWindow()
	{
		return assets_;
	}

	n_windowgame::window_game*
	window_manager::GetGameWindow()
	{
		return game_;
	}

	n_windowinspector::window_inspector*
	window_manager::GetInspectorWindow()
	{
		return inspector_;
	}
	
	/* ---各ウィンドウの参照--- */



	void window_manager::RenderAll()
	{
		for (auto& win : windows_)
		{
			win->Render();
		}
	}
}