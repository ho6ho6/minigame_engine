/*シーンウィンドウ*/
#include "include/window_editor/window_scene.hpp"
#include "include/render.hpp"	//フレームバッファ取得用
#include "include/assets/texture.hpp"
#include "include/assets/assets_manager/texture_manager.hpp"
#include "imgui.h"					//ImGui本体
#include <stdio.h>
#include <string>
#include <iostream>


/* --注1--
    ImGui の API ではウィンドウ局所座標と絶対座標の概念があり、
    GetCursorScreenPos() や SetCursorScreenPos() は絶対座標、GetCursorPos() や SetCursorPos() はウィンドウ局所座標を扱うが
    これら"局所座標と絶対座標の間には現在不整合が存在"するため、絶対座標を使うのが無難。
    (2024年6月時点の ImGui 1.89.8)

    尚これはImGuiのドキュメントにも明記されている
*/

// instance_texmag のアドレスが違うためにAssetがsceneに追加できない　これを直すにはtexture_manager.cppとwindow_scene.cppを同じ翻訳単位でコンパイルする必要がある

static ImVec2 ViewOffset = ImVec2(0.0f, 0.0f);

namespace n_windowscene
{

    void window_scene::Render()
    {

        //printf("[window_scene] instance_texmag addr=%p file=%s\n", (void*)&n_texturemanager::instance_texmag, __FILE__);


        ImGui::SetNextWindowPos(ImVec2(920, 0), ImGuiCond_Always);
        //ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);

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

        ImGuiIO& io = ImGui::GetIO();

        // 論理サイズとフレームバッファサイズ
        int logical_w = (int)avail.x;
        int logical_h = (int)avail.y;
        int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
        int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

        n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);

        // ヒット領域を先に作る（描画と同じサイズ・位置にするため cursor を取得）
        ImVec2 contentPos = ImGui::GetCursorScreenPos(); // 領域のスクリーン左上 ****注1****
        ImVec2 contentSize = avail;                       // content のサイズ（ヒット領域として使用）

        //### デバック
            
            ImVec2 mouse = ImGui::GetMousePos();
            ImDrawList* draw = ImGui::GetWindowDrawList();

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
            ImVec2 rect_a = contentPos;
            ImVec2 rect_b = ImVec2(contentPos.x + contentSize.x, contentPos.y + contentSize.y);
            draw->AddRect(rect_a, rect_b, IM_COL32(255, 0, 0, 200), 0.0f, 0, 2.0f); // 赤枠：ヒット領域

            draw->AddCircleFilled(mouse, 4.0f, IM_COL32(255, 255, 0, 200)); // 黄色：実マウス

            // マウスのローカル座標（ヒット領域左上を原点）
            ImVec2 local = ImVec2(mouse.x - contentPos.x, mouse.y - contentPos.y);

            // テクスチャ上のピクセル座標に変換（重要：desc.Width/Height を使う）
            float mappedTexX = 0.0f, mappedTexY = 0.0f;
            if (contentSize.x > 0.0f && contentSize.y > 0.0f && texW > 0 && texH > 0) {
                mappedTexX = local.x * (float)texW / contentSize.x;
                mappedTexY = local.y * (float)texH / contentSize.y;
            }

            // 再び ImGui 上に戻して、マッピング位置を描画（緑十字）
            ImVec2 mappedOnImage = ImVec2(contentPos.x + (mappedTexX / (float)texW) * contentSize.x,
                contentPos.y + (mappedTexY / (float)texH) * contentSize.y);
            draw->AddLine(ImVec2(mappedOnImage.x - 8, mappedOnImage.y), ImVec2(mappedOnImage.x + 8, mappedOnImage.y), IM_COL32(0, 255, 0, 200), 2.0f);
            draw->AddLine(ImVec2(mappedOnImage.x, mappedOnImage.y - 8), ImVec2(mappedOnImage.x, mappedOnImage.y + 8), IM_COL32(0, 255, 0, 200), 2.0f);
            
        //###

        {
            // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
            ImDrawList* draw = ImGui::GetWindowDrawList();
            const ImVec2 winPos = ImGui::GetWindowPos();
            const ImVec2 winSize = ImGui::GetWindowSize();
            draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);

            float centerX = winPos.x + winSize.x * 0.5f + ViewOffset.x;
            float centerY = winPos.y + winSize.y * 0.5f + ViewOffset.y;
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
        }


        ImGui::InvisibleButton("Scene_drag_area", contentSize, ImGuiButtonFlags_MouseButtonRight);


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
                // シーン座標に変換
                //ImVec2 scene_pos = ImGui::GetWindowPos();
				// シーン内ローカル座標
                ImVec2 local = ImVec2(mouse_pos.x - contentPos.x, mouse_pos.y - contentPos.y);

                ImGui::EndDragDropTarget();


				// 遅延キューへ登録（ImGui の外で処理する） クラッシュの原因回避
                m_PendingDrop.push_back(std::move(asset_name));
                m_PendingDropPos.push_back(local);

            }
            else
            {
				ImGui::EndDragDropTarget();
            }
        }

        bool isRightActive = ImGui::IsItemActive();     // 押されている間 true


        if (isRightActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right); // 累積 delta
            // 1フレーム分に変換して反映する
            ViewOffset.x -= drag.x;
            ViewOffset.y -= drag.y;
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測
        }



        for (size_t i = 0; i < m_PendingDrop.size(); i++)
        {
            const std::string& asset_name = m_PendingDrop[i];
            ImVec2 guiLocalPos = m_PendingDropPos[i];
            ImVec2 guiWindowPos = ImGui::GetWindowPos(); // シーンウィンドウの絶対位置
            
			//printf("[window_scene] Processing dropped asset: %s\n", asset_name.c_str());

            Texture* tex = n_texturemanager::instance_texmag.GetTextureName(asset_name);
            if (tex)
            {
                AddAssetToScene(tex, asset_name, guiLocalPos, guiWindowPos);
				//continue;
            }
            else
            {
                printf("[window_scene] Texture not found for dropped asset: %s\n", asset_name.c_str());
			}


            // 受け取った名前
            printf("[window_scene] Dropped raw name: '%s' (len=%zu)\n", asset_name.c_str(), asset_name.size());

            // 列挙（GetTextureNames() が pair<string, ...> を返すことを仮定）
            printf("[window_scene] Registered texture keys:\n");
            for (const auto& kv : n_texturemanager::instance_texmag.GetTextureNames()) {
                printf("  '%s'\n", kv.first.c_str());
            }

			//window_scene::Get().AddAssetToScene(tex, asset_name, guiLocalPos, guiWindowPos);
        }


        //printf("[window_scene/Render] m_SceneSprites.count=%zu\n", m_SceneSprites.size());

        // シーンオブジェクト一覧を走査
        for (const auto& sprite : m_SceneSprites)
        {
            if (!sprite.texture) continue;

            ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(sprite.texture->tx_id);
            if (!srv) {
                printf("[window_scene/Render/ERR] sprite '%s' has null srv\n", sprite.name.c_str());
                continue;
            }

            //printf("[window_scene/Render] sizeof(SceneSprite)=%zu offsetof(name)=%zu offsetof(texture)=%zu offsetof(pos_x)=%zu offsetof(width)=%zu offsetof(height)=%zu\n",
                //sizeof(SceneSprite),
                //offsetof(SceneSprite, name),
                //offsetof(SceneSprite, texture),
                //offsetof(SceneSprite, pos_x),
                //offsetof(SceneSprite, width),
                //offsetof(SceneSprite, height));

            // sprite.pos_x/pos_y はシーン内ピクセル座標（AddAssetToSceneで設定済み）
            ImVec2 screenPos = ImVec2(contentPos.x + (sprite.pos_x - ViewOffset.x),
                                      contentPos.y + (sprite.pos_y - ViewOffset.y));
            ImGui::SetCursorScreenPos(screenPos);

            ImVec2 drawSize = ImVec2((float)sprite.width, (float)sprite.height);
            // ログ出力（デバッグ用）
            //printf("[window_scene/Render/DBG] draw '%s' srv=%p pos=(%f,%f) size=(%d,%d)\n",
                //sprite.name.c_str(), (void*)srv, screenPos.x, screenPos.y, sprite.width, sprite.height);

            // テスト用 SRV を取得（既にロード済みの cube の SRV を流用）
            //ID3D11ShaderResourceView* testSrv = reinterpret_cast<ID3D11ShaderResourceView*>(m_SceneSprites.front().texture->tx_id);
            //ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 10));
            //ImGui::Image((ImTextureID)testSrv, ImVec2(16.0f, 16.0f));
            ////printf("[SMOKE TEST] testSrv=%p\n", (void*)testSrv);

            // ImGui_ImplDX11 は ImTextureID に SRV ポインタを渡す実装が標準
            ImGui::Image((ImTextureID)srv, drawSize);

            //ImVec2 a = screenPos;
            //ImVec2 b = ImVec2(screenPos.x + sprite.width, screenPos.y + sprite.height);
            //ImGui::GetWindowDrawList()->AddRect(a, b, IM_COL32(0, 255, 255, 200), 0.0f, 0, 2.0f);
            //printf("[CLIP TEST] rect a=(%f,%f) b=(%f,%f)\n", a.x, a.y, b.x, b.y);

            //printf("[SRVCHK] front.tx=%p front.tx_id=%p sprite.tex=%p sprite.tx_id=%p\n",
            //    (void*)m_SceneSprites.front().texture, (void*)m_SceneSprites.front().texture->tx_id,
            //    (void*)sprite.texture, (void*)sprite.texture->tx_id);


        }

        m_PendingDrop.clear();
        m_PendingDropPos.clear();
        ImGui::End();
    }



    void window_scene::AddAssetToScene(Texture* tex, const std::string& asset_name_str, ImVec2 guiLocalPos, ImVec2 guiWindowPos)
    {

        //printf("[AddAssetToScene] tex_ptr=%p tx_id=%p width=%d height=%d asset='%s' guiLocal=(%f,%f) guiWin=(%f,%f)\n",
            //(void*)tex, (void*)tex->tx_id, tex->width, tex->height,
            //asset_name_str.c_str(), guiLocalPos.x, guiLocalPos.y, guiWindowPos.x, guiWindowPos.y);

        if (!tex) { printf("[Window_scene/ERR] tex==nullptr\n"); return; }
        //printf("[Window_scene/DBG] tex->tx_id=%p\n", tex->tx_id);

        // シーンにアセットを追加する処理をここに実装
        ImGuiIO& io = ImGui::GetIO();

        // GUI論理座標をシーン座標に変換
        ImVec2 scale = io.DisplayFramebufferScale;

        // GUI　内の絶対座標
        ImVec2 guiAbsPos = ImVec2(guiWindowPos.x + guiLocalPos.x, guiWindowPos.y + guiLocalPos.y);

        float sceneX = guiAbsPos.x * scale.x;
        float sceneY = guiAbsPos.y * scale.y;

        // テクスチャサイズを使って配置（中心配置）
        int tex_w = tex->width > 0 ? tex->width : 16;
        int tex_h = tex->height > 0 ? tex->height : 16;
        float placeX = sceneX - tex_w * 0.5f;
        float placeY = sceneY - tex_h * 0.5f;

		SceneSprite sprite;
		sprite.name = asset_name_str;
		sprite.texture = tex;
		sprite.pos_x = placeX;
		sprite.pos_y = placeY;
		sprite.width = tex_w;
		sprite.height = tex_h;
        sprite.selected = true;

        m_SceneSprites.push_back(sprite);

        //printf("[window_scene/AddAssetToScene] pushed '%s' tex=%p size=(%d,%d) pos=(%f,%f)\n",
            //sprite.name.c_str(), (void*)sprite.texture, sprite.width, sprite.height, sprite.pos_x, sprite.pos_y);

    }

}

namespace n_texturemanager
{
    Texture* texture_manager::GetTextureName(const std::string& name)
    {
		auto it = m_Textures.find(name);    // nameで検索
		if (it == m_Textures.end()) return nullptr; // 見つからなかった場合はnullptrを返す
		return &it->second; // 見つかった場合はTextureへのポインタを返す
    }
}

