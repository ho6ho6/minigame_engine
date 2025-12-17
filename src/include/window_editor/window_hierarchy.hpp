#ifndef WINDOW_HIERARCHY
#define WINDOW_HIERARCHY

#pragma once
#include <imgui.h>
#include "window_base.hpp"
#include <string>
#include <vector>
#include "window_scene_sprite.hpp"
#include <functional>
#include <atomic>

using Command = std::function<void(int64_t objectID)>;

namespace n_windowhierarchy
{
	class window_hierarchy : public n_windowbase::window_base
	{
	public:
		void Render() override;
		void GetObjFromScene(const SceneToHierarchyObj& Obj);	// Sceneからオブジェクトを取得
		void DeleteObjFromScene(uint64_t id);	// Sceneからオブジェクト削除を取得

	private:
		ImVec2 m_LastSize = { 0,0 };

		// コンテキストメニュー用コマンド群
		std::vector<std::pair<std::string, Command>> contextMenuCommands;
	};


	// グローバルインスタンス sceneに配置されたAssetsをhierarchyに反映させるために使用
	window_hierarchy& instance_winHie();
}

#endif // !WINDOW_HIERARCHY