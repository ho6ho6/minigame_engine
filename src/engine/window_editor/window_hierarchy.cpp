#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/window_editor.h"
#include "include/window_editor/window_scene.h"
#include "include/assets/assets_manager/assets_manager.h"
#include "include/component/componentDefaults.h"
#include "include/render.h"	//フレームバッファ取得用
#include "include/game.h"
#include "include/component/component_api.h"
#include "include/component/component_keycapture.h"
#include "include/component/component_config.hpp"
#include "include/component/component_manager.hpp"
#include "imgui_impl_dx11.h"		//ImGuiでDirectX11

#include <mutex>

// objを保持するためのコンテナ
static std::vector<SceneToHierarchyObj> g_SceneToHierarchyObjs;
static std::mutex g_SceneToHierarchyMutex;
static std::mutex g_mutex;
static std::vector<int64_t> g_cachedSpriteIds;
static std::vector<EntitySpritePair> g_cachedPairs;
static bool g_cacheValid = false;
static std::unordered_map<int64_t, int64_t> g_entityToSprite;
static std::mutex g_registryMutex;

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



    void window_hierarchy::RegisterSprite(int64_t entityId, int64_t spriteId) {
        std::lock_guard<std::mutex> lk(g_registryMutex);
        g_entityToSprite[entityId] = spriteId;
    }

    void window_hierarchy::UnregisterSprite(int64_t entityId) {
        std::lock_guard<std::mutex> lk(g_registryMutex);
        g_entityToSprite.erase(entityId);
    }

    std::optional<int64_t> window_hierarchy::GetSpriteIdForEntity(int64_t entityId) {
        std::lock_guard<std::mutex> lk(g_registryMutex);
        auto it = g_entityToSprite.find(entityId);
        if (it == g_entityToSprite.end()) return std::nullopt;
        return it->second;
    }




    void window_hierarchy::OnHierarchyAdd(int64_t entityId, const n_component::SpriteComponent& sc)
    {
        SceneSprite s;
        s.id = sc.spriteId;
        s.pos_x = 0.0f;
        s.pos_y = 0.0f;
        s.selected = sc.visible;
        n_windowscene::instance_winSce().RegisterSprite(entityId, s);
        printf("[ui] OnHierarchyAdd entity=%lld spriteAsset=%lld\n", (long long)entityId, (long long)sc.spriteId);

        // 本登録はゲームスレッドへ（sc をコピーキャプチャ）
        n_gamecomponent::instance_gameFunctions().AddComponentSprite(entityId, sc);
    }

    static void RebuildCache()
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_cachedSpriteIds.clear();
        g_cachedPairs.clear();

        // 全エンティティ走査（n_compoapi::GetAllEntities() はそのまま使う）
        for (int64_t eid : n_compoapi::GetAllEntities()) {
            // eid が 0 を返す設計ならスキップ（通常は不要）
            if (eid == 0) continue;

            // ここでエンティティから Sprite コンポーネントを取得する
            // GetSpriteComponent は optional を返す想定
            auto spriteOpt = n_compoapi::GetSpriteComponent(eid);
            if (!spriteOpt) continue; // スプライトを持たないエンティティはスキップ

            // optional を展開してフィールドを取り出す
            auto sprite = *spriteOpt;
            int64_t spriteId = sprite.spriteId; // 実際のフィールド名に合わせる

            // ヒエラルキー上で「配置されている」かを判定するならここでチェック
            // 例: if (!sprite.visible) continue;

            g_cachedSpriteIds.push_back(spriteId);
            g_cachedPairs.emplace_back(eid, spriteId);
        }

        g_cacheValid = true;
    }

    std::vector<EntitySpritePair> window_hierarchy::GetAllVisibleEntitySpritePairs() {
        if (!g_cacheValid) RebuildCache();
        std::lock_guard<std::mutex> lk(g_mutex);
        return g_cachedPairs;
    }

    void NotifyHierarchyChanged() {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_cacheValid = false; // 次回取得時に再構築
    }

    void window_hierarchy::InitHierarchyScanner() {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_cacheValid = false;
    }

    void window_hierarchy::ShutdownHierarchyScanner() {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_cachedSpriteIds.clear();
        g_cachedPairs.clear();
        g_cacheValid = false;
    }

    void LoadInspectorForEntity(EntityId eid)
    {
        if (eid < 0)
        {
            // Inspectorをクリア
            g_selectedEntity = -1;
            return;
        }

        auto t = n_compoapi::GetTransformComponent(eid);
        auto m = n_compoapi::GetMoveComponent(eid);
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
        ImGui::SetWindowSize(ImVec2(450, 400));

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

        // まず「一度だけ登録」処理を行う（元データを書き換える必要がある）
        {
            std::lock_guard<std::mutex> lock(g_SceneToHierarchyMutex);
            for (auto& objRef : g_SceneToHierarchyObjs) {
                if (!objRef.registered) {
                    // 既にゲーム側や registry に登録されていないか二重チェック
                    if (!n_compoapi::HasSpriteComponent(objRef.id) &&
                        !window_hierarchy::GetSpriteIdForEntity(objRef.id).has_value()) {
                        n_component::SpriteComponent sc;
                        sc.spriteId = objRef.id;      // 必要なら別の asset id を設定
                        sc.visible = objRef.selected;
                        OnHierarchyAdd(objRef.id, sc); // 一度だけ呼ぶ
                    }
                    objRef.registered = true; // フラグを立てる（以降呼ばない）
                }
            }
            // 更新した g_SceneToHierarchyObjs を localObjs に反映しておく（描画で使うため）
            localObjs = g_SceneToHierarchyObjs;
        }


        for (const auto& obj : localObjs)
        {
			ImGui::PushID((int64_t)obj.id);
            bool isSelected = (g_selectedEntity == obj.id);

            if (ImGui::Selectable(obj.name.c_str(), isSelected))
            {
                // クリックで選択を切り替える（単一選択）
                if (g_selectedEntity != obj.id) {
                    g_selectedEntity = obj.id;
                    // Inspector を即時更新する処理をここで呼ぶ（関数化しておくと良い）
                    LoadInspectorForEntity(obj.id);
                }
                else {
                    g_selectedEntity = -1;
                    LoadInspectorForEntity(-1); // または ClearInspector()
                }
            }

            if (ImGui::BeginPopupContextItem())
            {
                /*もう少し可読性は上げる*/

                // contextCommands の登録はコンストラクタで行う想定
                // 直接呼ぶ（引数は id のみ）
                if (!n_compoapi::HasTransformComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Transform"))       n_compoapi::AddTransformComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove Transform"))    n_compoapi::RemoveTransformComponent(obj.id);
                }
                
                if (!n_compoapi::HasMoveComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Move"))            n_compoapi::AddMoveComponent(obj.id); // Moveが付与されていない
                }
                else
                {
                    if (ImGui::MenuItem("Remove Move"))         n_compoapi::RemoveMoveComponent(obj.id); // Moveが付与されている
                }

                if (!n_compoapi::HasLightComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Light"))           n_compoapi::AddLightComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove Light"))        n_compoapi::RemoveLightComponent(obj.id);
                }

                if (!n_compoapi::HasRigidbodyComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Rigidbody"))       n_compoapi::AddRigidbodyComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove Rigidbody"))    n_compoapi::RemoveRigidbodyComponent(obj.id);
                }

                if (!n_compoapi::HasStartComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Start"))           n_compoapi::AddStartComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove Start"))        n_compoapi::RemoveStartComponent(obj.id);
                }

                if (!n_compoapi::HasFinishComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add Finish"))          n_compoapi::AddFinishComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove Finish"))       n_compoapi::RemoveFinishComponent(obj.id);
                }

                if (!n_compoapi::HasIsPlayerComponent(obj.id))
                {
                    if (ImGui::MenuItem("Add IsPlayer"))        n_compoapi::AddIsPlayerComponent(obj.id);
                }
                else
                {
                    if (ImGui::MenuItem("Remove IsPlayer"))     n_compoapi::RemoveIsPlayerComponent(obj.id);
                }

				ImGui::EndPopup();
            }

			ImGui::PopID();
        }

        ImGui::End();


        ImGui::SetNextWindowPos(ImVec2(450, 600), ImGuiCond_Always);
        
        ImGui::Begin("Hierarchy Inspector"); // もしくは同じウィンドウ内で区切る
        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(470, 400));

        if (g_selectedEntity >= 0) {

            // コンポーネント用の Inspector を直接呼ぶ
            ImGui::Text("----Transform----");
            RenderTransformInspector(g_selectedEntity);

            ImGui::Text("----Move----");
            RenderMoveInspector(g_selectedEntity);

            ImGui::Text("----Rigidbody----");
            RenderRigidbodyInspector(g_selectedEntity);

            ImGui::Text("----IsPlayer----");
            RenderIsPlayerInspector(g_selectedEntity);

            ImGui::Text("----Start----");
            RenderStartInspector(g_selectedEntity);

            ImGui::Text("----Finish----");
            RenderFinishInspector(g_selectedEntity);

            ImGui::Text("----Light----");
            RenderLightInspector(g_selectedEntity);
            // キーキャプチャモーダルは毎フレームどこかで必ず呼ぶ（UI ループ内）
            RenderKeyCaptureModal();
        }
        else
        {
            ImGui::Text("No selection");
        }

        ImGui::End();

    }

}