#ifndef WINDOW_HIERARCHY
#define WINDOW_HIERARCHY

#pragma once
#include <imgui.h>
#include "window_base.hpp"
#include <string>
#include <vector>
#include "window_scene_sprite.hpp"
#include <functional>

namespace n_windowhierarchy
{
	class window_hierarchy : public n_windowbase::window_base
	{
	public:
		void Render() override;
		void GetObjFromScene();

	private:
		ImVec2 m_LastSize = { 0,0 };
	};

	// scenewindowに投下されたオブジェクトのヒエラルキークラス
	class hierarchy_object
	{
	public:
		std::vector<SceneSprite> nodes;		//シーン内のノード群
		std::function<void()> on_changed;	//ノード変更時に呼ばれるコールバック
		uint64_t NewId();					//ユニークID発行
		SceneSprite& AddNode(const std::string& name, Texture* tex, float worldX, float worldY, int w, int h);	//ノード追加
		void RemoveNode(uint64_t id);	//ノード削除
		void RenameNode(uint64_t id, const std::string& newName);	//ノード名変更
		SceneSprite* FindById(uint64_t id);	//IDでノード検索
	};

	static hierarchy_object& GetHierarchyObject()
	{
		static hierarchy_object instance;
		return instance;
	}
}

#endif // !WINDOW_HIERARCHY