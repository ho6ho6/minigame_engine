#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_scene_sprite.h"
#include "include/window_editor/scene/scene_ctx.h"
#include "include/window_editor/scene/scene_input.h"

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

static ImVec2 viewOffset = ImVec2(0.0f, 0.0f);
std::unordered_map<int64_t, SceneSprite> sprites;
std::mutex spriteMutex;

/* 当たり判定/選択/ドラッグは、ScreenToSceneを使用*/
/*
ImVec2 ScreenToScene(ImVec2 mouseScreen, ImVec2 contentPos, const ImVec2& viewOffset, const ImVec2& contentSize)
{
    ImGuiIO& io = ImGui::GetIO();
    // content 内のローカル座標(screen->content左上基準)
    ImVec2 local = ImVec2(mouseScreen.x - contentPos.x, mouseScreen.y - contentPos.y);

    // scene 論理ピクセル座標に変換  FramebufferScale を考慮
    ImVec2 localLogical = ImVec2(local.x / io.DisplayFramebufferScale.x,
        local.y / io.DisplayFramebufferScale.y);

    // 左上原点のワールド座標（ViewOffset は左上基準のパン）
    ImVec2 world = ImVec2(localLogical.x + viewOffset.x, localLogical.y + viewOffset.y);
    return world;
}

*/

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


        ImGui::SetNextWindowPos(ImVec2(920, 0), ImGuiCond_Always);

        ImGui::Begin("Scene_View", nullptr, ImGuiWindowFlags_NoResize);

        // window サイズを固定
        ImGui::SetWindowSize(ImVec2(1000, 600));

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

        // ヒット領域を先に作る（描画と同じサイズ・位置にするため cursor を取得）
        //ImVec2 contentPos = ImGui::GetCursorScreenPos(); // 領域のスクリーン左上 ****注1****
        //ImVec2 contentSize = avail;                       // content のサイズ（ヒット領域として使用）

        // mouseのscreen座標
        //ImVec2 mouseScreen = ImGui::GetMousePos();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        //ImVec2 mouseScene = ScreenToScene(mouseScreen, contentPos, viewOffset, contentSize);    // シーン座標に変換

        SceneViewContext ctx;
        BuildSceneViewContext(ctx);

        ctx.content.screenPos   = ImGui::GetCursorScreenPos();
        ctx.content.fbScale     = io.DisplayFramebufferScale;
        ctx.content.size        = avail;

        ctx.input.mouseScreen   = ImGui::GetMousePos();

        ctx.camera.viewOffset   = viewOffset;

        //### デバック

        // SRV (D3D11) からテクスチャサイズを取得
        int texW = 0, texH = 0;
        {
            ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(n_render::Render_GetSceneSRV());
            if (srv) {
                ID3D11Resource* res = nullptr;
                srv->GetResource(&res);
                if (res) {
                    ID3D11Texture2D* tex = nullptr;
                    if (SUCCEEDED(res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex)) && tex) {
                        D3D11_TEXTURE2D_DESC desc;
                        tex->GetDesc(&desc);
                        texW = desc.Width;
                        texH = desc.Height;
                        tex->Release();
                    }
                    res->Release();
                }
            }
        }

        // 視覚デバッグ：ヒット領域の枠とマウス位置
        ImVec2 rect_a = ctx.content.screenPos;
        ImVec2 rect_b = ImVec2(ctx.content.screenPos.x + ctx.content.size.x, ctx.content.screenPos.y + ctx.content.size.y);
        draw->AddRect(rect_a, rect_b, IM_COL32(255, 0, 0, 200), 0.0f, 0, 2.0f); // 赤枠：ヒット領域

        draw->AddCircleFilled(ctx.input.mouseScreen, 4.0f, IM_COL32(255, 255, 0, 200)); // 黄色：実マウス

        // マウスのローカル座標（ヒット領域左上を原点）
        ImVec2 localTex = ImVec2(ctx.input.mouseScreen.x - ctx.content.screenPos.x, ctx.input.mouseScreen.y - ctx.content.screenPos.y);

        // テクスチャ上のピクセル座標に変換（重要：desc.Width/Height を使う）
        float mappedTexX = 0.0f, mappedTexY = 0.0f;
        if (ctx.content.screenPos.x > 0.0f && ctx.content.screenPos.y > 0.0f && texW > 0 && texH > 0) {
            mappedTexX = (localTex.x / io.DisplayFramebufferScale.x) * (float)texW / ctx.content.screenPos.x;
            mappedTexY = (localTex.y / io.DisplayFramebufferScale.y) * (float)texH / ctx.content.screenPos.y;
        }

        // 再び ImGui 上に戻して、マッピング位置を描画（緑十字）
        ImVec2 mappedOnImage = ImVec2(ctx.content.screenPos.x + (mappedTexX / (float)texW) * ctx.content.screenPos.x,
                                      ctx.content.size.y + (mappedTexY / (float)texH) * ctx.content.size.y);
        draw->AddLine(ImVec2(mappedOnImage.x - 8, mappedOnImage.y), ImVec2(mappedOnImage.x + 8, mappedOnImage.y), IM_COL32(0, 255, 0, 200), 2.0f);
        draw->AddLine(ImVec2(mappedOnImage.x, mappedOnImage.y - 8), ImVec2(mappedOnImage.x, mappedOnImage.y + 8), IM_COL32(0, 255, 0, 200), 2.0f);

        //###


    // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
    //ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);

        float centerX = winPos.x + winSize.x * 0.5f - viewOffset.x;
        float centerY = winPos.y + winSize.y * 0.5f - viewOffset.y;
        ImVec2 origin(centerX, centerY);

        const float gridStep = 20.0f;
        const ImU32 colGrid = IM_COL32(200, 200, 200, 50);

        for (float x = fmodf(origin.x, gridStep); x < winPos.x + winSize.x; x += gridStep)
            draw->AddLine(ImVec2(x, origin.y - winSize.y * 0.5f), ImVec2(x, origin.y + winSize.y * 0.5f), colGrid);
        for (float y = fmodf(origin.y, gridStep); y < winPos.y + winSize.y; y += gridStep)
            draw->AddLine(ImVec2(origin.x - winSize.x * 0.5f, y), ImVec2(origin.x + winSize.x * 0.5f, y), colGrid);

        const ImU32 colAxisX = IM_COL32(255, 0, 0, 255);
        draw->AddLine(ImVec2(origin.x - winSize.x * 10.0f, origin.y), ImVec2(origin.x + winSize.x * 10.0f, origin.y), colAxisX, 2.0f);
        const ImU32 colAxisY = IM_COL32(0, 255, 0, 255);
        draw->AddLine(ImVec2(origin.x, origin.y - winSize.y * 10.0f), ImVec2(origin.x, origin.y + winSize.y * 10.0f), colAxisY, 2.0f);

        const ImU32 colText = IM_COL32(255, 255, 255, 200);
        draw->AddText(origin, colText, "(0,0)");

        draw->PopClipRect();


        // --- ヒット領域の InvisibleButton を作成 ---


        if (ctx.content.size.x <= 0.0f || ctx.content.size.y <= 0.0f)
        {
            ctx.content.size.x = 1.0f;
            ctx.content.size.y = 1.0f;
        }
        ImGui::InvisibleButton("Scene_drag_area", ctx.content.size, ImGuiButtonFlags_MouseButtonRight);


        // assetsのドラッグ＆ドロップを受け取る
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD"))
            {
                const char* data = reinterpret_cast<const char*>(payload->Data);
                int data_size = payload->DataSize;

                std::string asset_name(data, data + data_size); // 文字列化

                if (!asset_name.empty() && asset_name.back() == '\0') asset_name.pop_back();

                // マウス座標　論理座標
                ImVec2 mouse_pos = ImGui::GetMousePos();

                // 論理ピクセルで保存
                ImVec2 localLogical = ImVec2((mouse_pos.x - ctx.content.screenPos.x) / ctx.content.fbScale.x,
                                             (mouse_pos.y - ctx.content.screenPos.y) / ctx.content.fbScale.y);

                ImGui::EndDragDropTarget();


                // 遅延キューへ登録（ImGui の外で処理する） クラッシュの原因回避
                m_PendingDrop.push_back(std::move(asset_name));
                m_PendingDropPos.push_back(localLogical);

            }
            else
            {
                ImGui::EndDragDropTarget();
            }
        }



        bool isRightActive = ImGui::IsItemActive();     // 押されている間 true

        // 右クリックを動かす処理を論理ピクセルで
        if (isRightActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 dragFreamebuffer = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right); // 累積 delta

            //ImGuiIO& io = ImGui::GetIO();   // ImGuiIO を再取得

            // framebuffer -> 論理ピクセル変換
            float dragX = dragFreamebuffer.x / ctx.content.fbScale.x;
            float dragY = dragFreamebuffer.y / ctx.content.fbScale.y;

            viewOffset.x -= dragX;
            viewOffset.y -= dragY;

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測
        }

        //mouseScreen = ImGui::GetMousePos(); // 再取得
        ImVec2 mouseScene = input::ScreenToScene(ctx);    // シーン座標に変換

        // ヒット領域内か -> クリックがUIのその領域内で発生したかどうかの判定に使用 スクリーン座標で判定

        bool mouseInContent = (ctx.input.mouseScreen.x >= ctx.content.size.x) && (ctx.input.mouseScreen.x <= ctx.content.screenPos.x + ctx.content.size.x) &&
            (ctx.input.mouseScreen.y >= ctx.content.screenPos.y) && (ctx.input.mouseScreen.y <= ctx.content.screenPos.y + ctx.content.size.y);

        // --- クリックで選択 ---
        int clickedIndex = -1;
        if (mouseInContent && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Right)
            && ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
        {
            for (int i = (int)m_SceneSprites.size() - 1; i >= 0; --i)
            {
                const auto& s = m_SceneSprites[i];
                if (mouseScene.x >= s.pos_x && mouseScene.x <= s.pos_x + s.width &&
                    mouseScene.y >= s.pos_y && mouseScene.y <= s.pos_y + s.height)
                {
                    clickedIndex = i;
                    break;
                }
            }

            for (size_t i = 0; i < m_SceneSprites.size(); ++i)
            {
                m_SceneSprites[i].selected = ((int)i == clickedIndex);
                if (m_SceneSprites[i].selected)
                {
                    // ドラッグオフセットを記録
                    m_SceneSprites[i].dragOffsetX = mouseScene.x - m_SceneSprites[i].pos_x;
                    m_SceneSprites[i].dragOffsetY = mouseScene.y - m_SceneSprites[i].pos_y;
                }
            }

            // window_scene で直接選択されても hierarchy に反映させる
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

        // ドラッグ移動は選択済みのみに
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 curMouseScene = input::ScreenToScene(ctx);
            for (auto& s : m_SceneSprites)
            {
                if (!s.selected) continue;
                float newX = curMouseScene.x - s.dragOffsetX;
                float newY = curMouseScene.y - s.dragOffsetY;

                // Transform 更新
                auto tOpt = n_compoapi::GetTransformComponent(s.id);
                if (tOpt)
                {
                    auto t = *tOpt;
                    t.position[0] = newX;
                    t.position[1] = newY;
                    n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid = s.id, t]() {
                        n_compoapi::SetTransformComponent(eid, t);
                        });
                }
            }
        }



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
                AddAssetToScene(tex, asset_name, guiLocalPos, guiWindowPos, ctx.content.size);    // sceneに追加
            }
            else
            {
                printf("[window_scene] Texture not found for dropped asset: %s\n", asset_name.c_str());
            }


            // 受け取った名前
            // printf("[window_scene] Dropped raw name: '%s' (len=%zu)\n", asset_name.c_str(), asset_name.size());

            // 列挙（GetTextureNames() が pair<string, ...> を返すことを仮定）
            //printf("[window_scene] Registered texture keys:\n");
            //for (const auto& kv : n_texturemanager::instance_texmag().GetTextureNames()) {
            //    printf("  '%s'\n", kv.first.c_str());
            //}

        }


        //printf("[window_scene/Render] m_SceneSprites.count=%zu\n", m_SceneSprites.size());

        // シーンオブジェクト一覧を走査
        for (auto& sprite : m_SceneSprites)
        {
            if (!sprite.texture) continue;

            //ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(sprite.texture->tx_id);
            //if (!srv) {
            //    printf("[window_scene/Render/ERR] sprite '%s' has null srv\n", sprite.name.c_str());
            //    continue;
            //}


            // Transform Component から座標を取得
            auto tOpt = n_compoapi::GetTransformComponent(sprite.id);
            if (tOpt)
            {
                sprite.pos_x = tOpt->position[0];
                sprite.pos_y = tOpt->position[1];
                //printf("[window_scene] Transformからposition取得成功 \n");
            }

            // sprite.pos_x/pos_y はシーン内ピクセル座標（AddAssetToSceneで設定済み）
            ImVec2 screenPos = ImVec2(ctx.content.screenPos.x + (sprite.pos_x - viewOffset.x) * ctx.content.fbScale.x,
                                      ctx.content.screenPos.y + (sprite.pos_y - viewOffset.y) * ctx.content.fbScale.y);
            ImGui::SetCursorScreenPos(screenPos);


            ImVec2 drawSize = ImVec2((float)sprite.width, (float)sprite.height);

            // 左クリックで選択され && ドラッグ中
            if (sprite.selected && ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
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
                    DeleteAssetFromScene(toDelete);
                    SetSelectedEntity(NOT_SELECTED);
                }
            }

            // ### デバッグ情報表示
            //printf("[window_scene/Render/DBG] localTex=(%f,%f) mouseScene=(%f,%f) contentPos=(%f,%f) mouseScreen=(%f,%f) ViewOffset=(%f,%f) sprite.pos=(%f,%f)\n",
            //    localTex.x, localTex.y, mouseScene.x, mouseScene.y, contentPos.x, contentPos.y, mouseScreen.x, mouseScreen.y,ViewOffset.x, ViewOffset.y, sprite.pos_x, sprite.pos_y);

            //printf("[window_scene/Render/DBG] sprite='%s' pos=(%f,%f) dragOffset=(%f,%f) selected=%d\n",
            //    sprite.name.c_str(), sprite.pos_x, sprite.pos_y, sprite.dragOffsetX, sprite.dragOffsetY, sprite.selected);


            // ImGui_ImplDX11はImTextureIDにSRV ポインタを渡す実装が標準
            ImGui::Image(sprite.texture->tx_id, drawSize);
        }

        m_PendingDrop.clear();
        m_PendingDropPos.clear();
        ImGui::End();
    }



    void window_scene::AddAssetToScene(Texture* tex, const std::string& asset_name_str, ImVec2 guiLocalPos, ImVec2 guiWindowPos, const ImVec2& ContentSize)
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
        float sceneX = guiLocalPos.x + viewOffset.x;
        float sceneY = guiLocalPos.y + viewOffset.y;

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


    void window_scene::DeleteAssetFromScene(EntityId eid)
    {
        if (eid == NOT_SELECTED) return;
        printf("[window_scene/DeletAsFrScene] DeleteObjFromScene id=%llu\n", (signed long long)eid);

        // コンテナから削除 EntityIdで検索　添え字で削除するとズレてしまい、ヒエラルキーにオブジェクトが残る
        m_SceneSprites.erase(
            std::remove_if(
                m_SceneSprites.begin(),
                m_SceneSprites.end(),
                [eid](const SceneSprite& s)
                {
                    return s.id == eid;
                }),
            m_SceneSprites.end()
        );

        // ヒエラルキーウィンドウへ
        n_hierarchy::sync::OnSceneObjectRemoved(eid);
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