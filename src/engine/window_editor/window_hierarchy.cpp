#include "include/window_editor/window_hierarchy.hpp"
#include "include/window_editor/window_editor.hpp"
#include "include/render.hpp"	//フレームバッファ取得用
#include "include/game.hpp"
#include "include/game_component.hpp"
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11

#include <mutex>

// objを保持するためのコンテナ
static std::vector<SceneToHierarchyObj> g_SceneToHierarchyObjs;
static std::mutex g_SceneToHierarchyMutex;

namespace n_windowhierarchy
{
	// グローバルインスタンス
    /*もし使うことがあったら*/
	window_hierarchy& instance_winHie()
    {
        static window_hierarchy inst;
        return inst;
    }

	// Sceneからオブジェクトを取得
    void window_hierarchy::GetObjFromScene(const SceneToHierarchyObj& Obj)
    {
		// スレッドセーフに追加
		std::lock_guard<std::mutex> lock(g_SceneToHierarchyMutex);
		g_SceneToHierarchyObjs.push_back(Obj);
    }

	// Sceneからオブジェクト削除を取得
    void window_hierarchy::DeleteObjFromScene(uint64_t id)
    {
        if (id == 0) return;
		std::lock_guard<std::mutex> lock(g_SceneToHierarchyMutex);
        g_SceneToHierarchyObjs.erase(
            std::remove_if(g_SceneToHierarchyObjs.begin(), g_SceneToHierarchyObjs.end(),
                [id](const SceneToHierarchyObj& obj) { return obj.id == id; }),
            g_SceneToHierarchyObjs.end()
        );
    }

    void window_hierarchy::Render()
    {
        
        /*------------------------------------------*/

        /*windowの座標とサイズ*/
        ImGui::SetNextWindowPos(ImVec2(0, 600), ImGuiCond_Always);

        //ImGui::SetNextWindowSizeConstraints(ImVec2(400, 435), ImVec2(400, 480));

        // Window表示に関して
        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize);

        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(920, 400));

        ImVec2 avail = ImGui::GetContentRegionAvail();

        // サイズ変更の検出　安全策として 0 以下は無視
        if (avail.x <= 0.0f || avail.y <= 0.0f)
        {
            if (m_LastSize.x <= 0.0f || m_LastSize.y <= 0.0f) m_LastSize = ImVec2(1.0f, 1.0f);
        }
        else if (avail.x != m_LastSize.x || avail.y != m_LastSize.y)
        {
            m_LastSize = avail;
        }

        ImGuiIO& io = ImGui::GetIO();

        // 論理サイズとフレームバッファサイズ
        int logical_w = (int)avail.x;
        int logical_h = (int)avail.y;
        int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
        int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

        n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);

        /*------------------------------------------*/

		// 描画時にローカルコピーを作成しておく
		std::vector<SceneToHierarchyObj> localObjs;
        {
			std::lock_guard<std::mutex> lock(g_SceneToHierarchyMutex);
			localObjs = g_SceneToHierarchyObjs;
        }

        for (const auto& obj : localObjs)
        {
			ImGui::PushID((int64_t)obj.id);
			bool selected = obj.selected;
            if (ImGui::Selectable(obj.name.c_str(), &selected))
            {
				// 選択処理　ノードの追加はここで行う
            }


            if (ImGui::BeginPopupContextItem())
            {
                /*もう少し可読性は上げる*/

                // contextCommands の登録はコンストラクタで行う想定
                if (ImGui::MenuItem("Add Transform")) {
                    // 直接呼ぶ（引数は id のみ）
                    n_gamecomponent::instance_gameFunctions().AddComponentTransform(obj.id);

                } else if (ImGui::MenuItem("Add Move"))
                {
                    n_gamecomponent::instance_gameFunctions().AddComponentMove(obj.id);

                } else if (ImGui::MenuItem("Add Light"))
                {

                    n_gamecomponent::instance_gameFunctions().AddComponentLight(obj.id);

                } else if (ImGui::MenuItem("Add Gravity"))
                {

                    n_gamecomponent::instance_gameFunctions().AddComponentGravity(obj.id);

                } else if (ImGui::MenuItem("Object Delete"))
                {

                    n_gamecomponent::instance_gameFunctions().AddComponentDelete(obj.id);

				} else if (ImGui::MenuItem("Add Start"))
                {

                    n_gamecomponent::instance_gameFunctions().AddComponentStart(obj.id);

                } else if (ImGui::MenuItem("Add Finish"))
                {

                    n_gamecomponent::instance_gameFunctions().AddComponentFinish(obj.id);
                }
                else if (ImGui::MenuItem("Add isPlayer"))
                {
                    n_gamecomponent::instance_gameFunctions().AddComponentIsPlayer(obj.id);
                }
                /*Add Component*/

				ImGui::EndPopup();
            }
			ImGui::PopID();
        }

        ImGui::End();
    }

}