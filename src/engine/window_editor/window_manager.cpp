// window_manager.cpp

#include "../../include/window_editor/window_manager.hpp"
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/window_editor/window_game.hpp"
#include "../../include/window_editor/window_hierarchy.hpp"
#include "../../include/window_editor/window_assets.hpp"
//#include "../../include/window_editor/window_input.hpp"

namespace n_windowmanager
{
	void window_manager::Register_SceneWindow()
	{
		windows_.emplace_back(std::make_unique<n_windowscene::window_scene>());
	}

	void window_manager::Register_GameWindow()
	{
		windows_.emplace_back(std::make_unique<n_windowgame::window_game>());
	}

	/*void window_manager::Register_InputWindow()
	{
		windows_.emplace_back(std::make_unique<n_windowinput::window_input>());
	}*/
	
	void window_manager::Register_Hierarchywindow()
	{
		windows_.emplace_back(std::make_unique<n_windowhierarchy::window_hierarchy>());
	}

	void window_manager::Register_Assetswindow()
	{
		windows_.emplace_back(std::make_unique<n_windowassets::window_assets>());
	}

	void window_manager::RenderAll()
	{
		for (auto& win : windows_)
		{
			win->Render();
		}
	}
}