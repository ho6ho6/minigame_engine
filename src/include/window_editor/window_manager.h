/*各editorウィンドウの生成・削除を一元管理*/

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include <memory>
#include "window_base.h"
//#include "window_scene.h"
//#include "window_hierarchy.h"
//#include "window_assets.h"
//#include "window_game.h"
//#include "window_inspector.hpp"

// 前方宣言
namespace n_windowhierarchy { class window_hierarchy; }
namespace n_windowscene { class window_scene; }
namespace n_windowassets { class window_assets; }
namespace n_windowgame { class window_game; }
namespace n_windowinspector { class window_inspector; }

namespace n_windowmanager
{
    class window_manager
    {
        public:
            void Register_SceneWindow();
            void Register_GameWindow();
            void Register_Hierarchywindow();
            void Register_Assetswindow();
		    void Register_Inspectorwindow();

            void RenderAll();

		    // 各ウィンドウへのアクセス
            n_windowhierarchy::window_hierarchy* GetHierarchyWindow();
            n_windowscene::window_scene* GetSceneWindow();
            n_windowassets::window_assets* GetAssetsWindow();
		    n_windowgame::window_game* GetGameWindow();
		    n_windowinspector::window_inspector* GetInspectorWindow();

        private:
            n_windowhierarchy::window_hierarchy* hierarchy_ = nullptr;
            n_windowscene::window_scene* scene_ = nullptr;
            n_windowassets::window_assets* assets_ = nullptr;
		    n_windowgame::window_game* game_ = nullptr;
		    n_windowinspector::window_inspector* inspector_ = nullptr;

            std::vector<std::unique_ptr<n_windowbase::window_base>> windows_;
    };
}

#endif // !WINDOW_MANAGER_H