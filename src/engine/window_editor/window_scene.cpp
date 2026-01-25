#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_scene_sprite.h"
#include "include/window_editor/scene/scene_ctx.h"
#include "include/window_editor/scene/scene_input.h"
#include "include/window_editor/scene/scene_data.h"
#include "include/window_editor/scene/scene_ui.h"

#include "include/render.h"	//フレームバッファ取得用

#include "include/assets/texture.h"
#include "include/assets/assets_manager/texture_manager.hpp"

#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"

#include "include/component/component_api.h"
#include "include/component/game_component.h"

#include "imgui.h"  //ImGui本体

#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
/* --注1--
    ImGui の API ではウィンドウ局所座標と絶対座標の概念があり、
    GetCursorScreenPos() や SetCursorScreenPos() は絶対座標、GetCursorPos() や SetCursorPos() はウィンドウ局所座標を扱うが
    これら"局所座標と絶対座標の間には現在不整合が存在"するため、絶対座標を使うのが無難。
    (2024年6月時点の ImGui 1.89.8)

    尚これはImGuiのドキュメントにも明記されている

    instance_texmag のアドレスが違うためにAssetがsceneに追加できない　これを直すにはtexture_manager.cppとwindow_scene.cppを同じ翻訳単位でコンパイルする必要がある //
    screen座標のマウス位置をscene内の論理ピクセルに変換する
    contentPos:ImGui::GetCursorScreenPos() で取得した領域のスクリーン左上座標を使う
    ViewOffset:論理ピクセル単位で管理
    ScreenToSceneは、UI->テクスチャマッピングを計算する際には、スクリーン->コンテントローカル-> ÷スケール -> の順で行う テクスチャ上のピクセル位置はViewOffsetを含めない

    ↑の理由
    スクリーン位置:画面全体のピクセル位置
    コンテントローカル位置:ウィンドウ内のピクセル位置
    論理ピクセル(ImGui座標)とフレームバッファピクセルの違い:高DPI対応のため、論理ピクセルはフレームバッファピクセルと異なる場合がある
    scene座標:シーン内のピクセル位置、ViewOffsetを含む。ズームや移動で変換する
*/

std::unordered_map<int64_t, SceneSprite> sprites;
std::mutex spriteMutex;

namespace n_windowscene
{

    int64_t window_scene::GenerateUniqueSpriteId()
    {
        return g_NextSpriteId++;
    }

    window_scene& instance_winSce()
    {
        static window_scene inst;
        return inst;
    }

    void window_scene::Render()
    {
        /*
        ImGui::SetNextWindowPos(ImVec2(920, 0), ImGuiCond_Always);

        ImGui::Begin("Scene_View", nullptr, ImGuiWindowFlags_NoResize);

        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(1000, 600));
        */

        ui::SetupScene();

        ImVec2 avail = ImGui::GetContentRegionAvail();  // 利用可能な領域を取得

        // サイズ変更の検出　安全策として 0 以下は無視
        if (avail.x <= 0.0f || avail.y <= 0.0f)
        {
            if (m_LastSize.x <= 0.0f || m_LastSize.y <= 0.0f) m_LastSize = ImVec2(1.0f, 1.0f);
        }
        else if (avail.x != m_LastSize.x || avail.y != m_LastSize.y)
        {
            m_LastSize = avail;
        }

        // ビューポートのリサイズ
        ImGuiIO& io = ImGui::GetIO();

        // 論理ピクセルとフレームバッファサイズ
        int logical_w = (int)avail.x;
        int logical_h = (int)avail.y;
        int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
        int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

        n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);


        SceneViewContext ctx;
        BuildSceneViewContext(ctx);

        ctx.content.screenPos   = ImGui::GetCursorScreenPos();
        ctx.content.fbScale     = io.DisplayFramebufferScale;
        ctx.content.size        = avail;

        ctx.input.mouseScreen   = ImGui::GetMousePos();

        ctx.camera              = &m_camera;

        // ヒット領域を先に作る（描画と同じサイズ・位置にするため cursor を取得）
        ImDrawList* draw = ImGui::GetWindowDrawList();

        //### デバック
        ui::DebugSceneFrame(ctx, draw);
        //###


        // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
        
        ui::DrawSceneGrid(ctx, draw);

        // --- ヒット領域の InvisibleButton を作成 ---


        if (ctx.content.size.x <= 0.0f || ctx.content.size.y <= 0.0f)
        {
            ctx.content.size.x = 1.0f;
            ctx.content.size.y = 1.0f;
        }
        ImGui::InvisibleButton("Scene_drag_area", ctx.content.size, ImGuiButtonFlags_MouseButtonRight);


        // assetsのドラッグ＆ドロップを受け取る
        data::EnqueuePayloadFromAssetsWindow(ctx, m_PendingDrop, m_PendingDropPos);


        // カメラ視点
        input::HandlePan(ctx);

        // ヒット領域内か -> クリックがUIのその領域内で発生したかどうかの判定に使用 スクリーン座標で判定

        bool mouseInContent = (ctx.input.mouseScreen.x >= ctx.content.size.x) && (ctx.input.mouseScreen.x <= ctx.content.screenPos.x + ctx.content.size.x) &&
                              (ctx.input.mouseScreen.y >= ctx.content.screenPos.y) && (ctx.input.mouseScreen.y <= ctx.content.screenPos.y + ctx.content.size.y);

        // --- クリックで選択 ---
        int clickedIndex = NOT_SELECTED;
        if (mouseInContent && ctx.input.leftClicked && !ctx.input.rightDown && ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
        {
            ctx.input.isSceneCaptured = true;
            clickedIndex = input::HandleClick(ctx, mouseInContent, m_SceneSprites);

            if (clickedIndex >= 0)
            {
                SetSelectedEntity(m_SceneSprites[clickedIndex].id);
            }
            else
            {
                SetSelectedEntity(NOT_SELECTED);
            }
        }

        // --- ドラッグ移動 ---

        input::ApplyTransformToECS(ctx, m_SceneSprites);

        // --- ドロップされたアセットを処理 ---

        for (size_t i = 0; i < m_PendingDrop.size(); i++)
        {
            const std::string& asset_name = m_PendingDrop[i];
            ImVec2 guiLocalPos = m_PendingDropPos[i];
            ImVec2 guiWindowPos = ImGui::GetWindowPos(); // シーンウィンドウの絶対位置

            //printf("[window_scene] Processing dropped asset: %s\n", asset_name.c_str());

            Texture* tex = n_texturemanager::instance_texmag().GetTextureByName(asset_name);
            if (tex)
            {
                AddAssetToScene(tex, asset_name, guiLocalPos, guiWindowPos, ctx);    // sceneに追加
            }
            else
            {
                printf("[window_scene] Texture not found for dropped asset: %s\n", asset_name.c_str());
            }

        }


        // シーンオブジェクト一覧を走査
        for (auto& sprite : m_SceneSprites)
        {
            if (!sprite.texture) continue;

            // Transform Component から座標を取得
            auto tOpt = n_compoapi::GetTransformComponent(sprite.id);
            if (tOpt)
            {
                sprite.pos_x = tOpt->position[0];
                sprite.pos_y = tOpt->position[1];
                //printf("[window_scene] Transformからposition取得成功 \n");
            }

            // sprite.pos_x/pos_y はシーン内ピクセル座標（AddAssetToSceneで設定済み）
            ImVec2 screenPos = ImVec2(ctx.content.screenPos.x + (sprite.pos_x - ctx.camera->viewOffset.x) * ctx.content.fbScale.x,
                                      ctx.content.screenPos.y + (sprite.pos_y - ctx.camera->viewOffset.y) * ctx.content.fbScale.y);
            ImGui::SetCursorScreenPos(screenPos);


            ImVec2 drawSize = ImVec2((float)sprite.width, (float)sprite.height);

            // 左クリックで選択され && ドラッグ中
            if (sprite.selected && ctx.input.leftDown && ctx.input.leftDragging)
            {
                // 再計算して現在のマウスのscene座標を取得
                ImVec2 curMouseScene = input::ScreenToScene(ctx);
                sprite.pos_x = curMouseScene.x - sprite.dragOffsetX;    // ドラッグオフセットを考慮
                sprite.pos_y = curMouseScene.y - sprite.dragOffsetY;
            }


            // --- 選択された状態でEscapeキーが押されたら削除 --- 

            // 押下フレームのみ検出
            if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            {
                printf("[window_scene] EscapeKey down \n");
                EntityId toDelete = selectedEntity_;
                if (toDelete != NOT_SELECTED)
                {
                    printf("[window_scene] Deleting selected entity id=%lld \n", toDelete);
                    data::DeleteAssetFromScene(toDelete, m_SceneSprites);
                    SetSelectedEntity(NOT_SELECTED);
                }
            }

            // ImGui_ImplDX11はImTextureIDにSRV ポインタを渡す実装が標準
            ImGui::Image(sprite.texture->tx_id, drawSize);
        }

        // 入力処理が camera を変更したら戻す
        m_camera.viewOffset = ctx.camera->viewOffset;

        m_PendingDrop.clear();
        m_PendingDropPos.clear();
        ImGui::End();
    }



    void window_scene::AddAssetToScene(Texture* tex, const std::string& asset_name_str, ImVec2 guiLocalPos, ImVec2 guiWindowPos, const SceneViewContext& ctx)
    {

        //printf("[AddAssetToScene] tex_ptr=%p tx_id=%p width=%d height=%d asset='%s' guiLocal=(%f,%f) guiWin=(%f,%f)\n",
            //(void*)tex, (void*)tex->tx_id, tex->width, tex->height,
            //asset_name_str.c_str(), guiLocalPos.x, guiLocalPos.y, guiWindowPos.x, guiWindowPos.y);

        if (!tex) { printf("[Window_scene/AddAssetToScene/ERR] tex==nullptr\n"); return; }
        //printf("[Window_scene/DBG] tex->tx_id=%p\n", tex->tx_id);

        // シーンにアセットを追加する処理をここに実装
        ImGuiIO& io = ImGui::GetIO();

        // GUI論理座標をシーン座標に変換
        ImVec2 fbscale = io.DisplayFramebufferScale;

        // GUI　内の絶対座標
        ImVec2 guiAbsPos = ImVec2(guiWindowPos.x + guiLocalPos.x, guiWindowPos.y + guiLocalPos.y);

        // scene座標(=論理ピクセル)
        float sceneX = guiLocalPos.x + ctx.camera->viewOffset.x;
        float sceneY = guiLocalPos.y + ctx.camera->viewOffset.y;

        // 実ピクセル -> 論理ピクセルに変換
        float logicalW = (float)(tex->width > 0 ? tex->width : 16) / fbscale.x;
        float logicalH = (float)(tex->height > 0 ? tex->height : 16) / fbscale.y;

        // 最大表示サイズ
        const float maxDisplay = 64.0f; // 例: 64x64 を上限にする
        float scale = 1.0f;
        if (logicalW > maxDisplay || logicalH > maxDisplay)
        {
            scale = std::min<float>(maxDisplay / logicalW, maxDisplay / logicalH);
        }
        int tex_w = (int)std::round(logicalW * scale);
        int tex_h = (int)std::round(logicalH * scale);

        // 中心配置: scene 座標は左上原点なので、テクスチャの半分を引く
        float placeX = sceneX - tex_w * 0.5f;
        float placeY = sceneY - tex_h * 0.5f;

        /*spiteは中心座標で処理しているため、他は左上中心だがこれを考慮して作成している*/

        SceneSprite sprite;
        sprite.name = asset_name_str;
        sprite.texture = tex;
        sprite.pos_x = placeX;  // 論理ピクセル座標
        sprite.pos_y = placeY;  // 論理ピクセル座標
        sprite.width = (int)std::round(tex_w);  // 論理ピクセル幅
        sprite.height = (int)std::round(tex_h); // 論理ピクセル高さ
        sprite.selected = false;    // 初期選択状態
        sprite.z_order = sprite.z_order + 1; // 最前面に
        sprite.id = GenerateUniqueSpriteId();

        //printf("[AddAssetToScene] name=%s texPx=(%d,%d) logical=(%d,%d) pos=(%f,%f)\n",
        //    asset_name_str.c_str(), tex->width, tex->height, sprite.width, sprite.height, sprite.pos_x, sprite.pos_y);

        m_SceneSprites.push_back(sprite);

        SceneToHierarchyObj Obj;
        Obj.name = asset_name_str;
        Obj.texture = tex;
        Obj.x = sprite.pos_x;
        Obj.y = sprite.pos_y;
        Obj.width = sprite.width;
        Obj.height = sprite.height;
        Obj.z_order = sprite.z_order;
        Obj.selected = sprite.selected;
        Obj.id = sprite.id; // 一意のIDとしてポインタを使用

        printf("[Scene] pushing sprite id=%lld name=%s selected=%d\n", (long long)sprite.id, asset_name_str.c_str(), (int)sprite.selected);

        // ヒエラルキーウィンドウへ
        n_hierarchy::sync::OnSceneObjectAdded(Obj);
        printf("[Scene] OnSceneObjectAdded returned for id=%lld\n", (long long)Obj.id);
    }


    /* 新しいwindow_scene関数 */
    void window_scene::SetSelectedEntity(EntityId eid)
    {
        if (selectedEntity_ == eid) return; // 変更なし

        // 選択状態を更新
        selectedEntity_ = eid;
        printf("[Scene] SetSelectedEntity eid=%lld\n", (long long)eid);

        // hierarchyへ通知
        if (windowManager_)
        {
            auto* hierarchy = windowManager_->GetHierarchyWindow();
            if (hierarchy)
            {
                hierarchy->GetHierarchyModel().SetSelectedEntityFromScene(eid);
            }
        }

    }


    EntityId window_scene::GetSelectedEntity() const
    {
        return selectedEntity_;
    }

    bool window_scene::GetSpritePosition(EntityId eid, std::array<float, 2>& outPos) const
    {
        auto it = sprites.find(eid);
        if (it == sprites.end()) return false;
        const SceneSprite& s = it->second;
        outPos[0] = s.pos_x;
        outPos[1] = s.pos_y;
        return true;
    };

    bool window_scene::SetSpritePosition(EntityId eid, const std::array<float, 2>& SetPos)
    {
        std::lock_guard<std::mutex> lk(spriteMutex);
        auto it = sprites.find(eid);
        if (it == sprites.end()) {
            printf("[winScene/SetSpritePos] SetSpritePosition: sprite not found for eid=%lld\n", (long long)eid);
            return false;
        }
        it->second.pos_x = SetPos[0];
        it->second.pos_y = SetPos[1];
        return true;
    }

    void window_scene::RegisterSprite(EntityId id, const SceneSprite& sprite)
    {
        std::lock_guard<std::mutex> lk(spriteMutex);
        sprites[id] = sprite;
        // 位置は Transform に従うので pos_x/pos_y は使わない
    }
}