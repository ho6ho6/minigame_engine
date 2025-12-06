/*シーンウィンドウ*/
#include "include/window_editor/window_scene.hpp"
#include "include/render.hpp"	//フレームバッファ取得用
#include "include/assets/texture.hpp"
#include "include/assets/assets_manager/texture_manager.hpp"
#include "include/window_editor/window_hierarchy.hpp"
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

// screen座標のマウス位置をscene内の論理ピクセルに変換する
// contentPos:ImGui::GetCursorScreenPos() で取得した領域のスクリーン左上座標を使う
// ViewOffset:論理ピクセル単位で管理
/* ScreenToSceneは、UI->テクスチャマッピングを計算する際には、スクリーン->コンテントローカル-> ÷スケール -> の順で行う テクスチャ上のピクセル位置はViewOffsetを含めない*/
/* 
    ↑の理由
	スクリーン位置:画面全体のピクセル位置
	コンテントローカル位置:ウィンドウ内のピクセル位置
	論理ピクセル(ImGui座標)とフレームバッファピクセルの違い:高DPI対応のため、論理ピクセルはフレームバッファピクセルと異なる場合がある
	scene座標:シーン内のピクセル位置、ViewOffsetを含む。ズームや移動で変換する
*/
/* 当たり判定/選択/ドラッグは、ScreenToSceneを使用*/
ImVec2 ScreenToScene(ImVec2 mouseScreen, ImVec2 contentPos, const ImVec2& viewOffset)
{
    ImGuiIO& io = ImGui::GetIO();
    // content 内のローカル座標(screen->content左上基準)
    ImVec2 local = ImVec2(mouseScreen.x - contentPos.x, mouseScreen.y - contentPos.y);

    // scene 論理ピクセル座標に変換  FramebufferScale を考慮
    ImVec2 localLogical = ImVec2(local.x / io.DisplayFramebufferScale.x,
                                 local.y / io.DisplayFramebufferScale.y);

    //ViewOffset を論理ピクセルで管理して加算 = scene座標
    ImVec2 scenePos = ImVec2(localLogical.x + viewOffset.x, // これで正しい
                             localLogical.y + viewOffset.y);//

    return scenePos;
}

namespace n_windowscene
{

    void window_scene::Render()
    {

        //printf("[window_scene/Render/DBG] instance_texmag addr=%p file=%s\n", (void*)&n_texturemanager::instance_texmag, __FILE__);


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

		// ビューポートのリサイズ
        ImGuiIO& io = ImGui::GetIO();

        // 論理ピクセルとフレームバッファサイズ
        int logical_w = (int)avail.x;
        int logical_h = (int)avail.y;
        int fb_w = (int)roundf(avail.x * io.DisplayFramebufferScale.x);
        int fb_h = (int)roundf(avail.y * io.DisplayFramebufferScale.y);

        n_render::Render_Resizeviewport(logical_w, logical_h, fb_w, fb_h);

        // ヒット領域を先に作る（描画と同じサイズ・位置にするため cursor を取得）
        ImVec2 contentPos = ImGui::GetCursorScreenPos(); // 領域のスクリーン左上 ****注1****
        ImVec2 contentSize = avail;                       // content のサイズ（ヒット領域として使用）
            
        // mouseのscreen座標
        ImVec2 mouseScreen = ImGui::GetMousePos();
        ImDrawList* draw = ImGui::GetWindowDrawList();

		ImVec2 mouseScene = ScreenToScene(mouseScreen, contentPos, ViewOffset);    // シーン座標に変換

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
            ImVec2 rect_a = contentPos;
            ImVec2 rect_b = ImVec2(contentPos.x + contentSize.x, contentPos.y + contentSize.y);
            draw->AddRect(rect_a, rect_b, IM_COL32(255, 0, 0, 200), 0.0f, 0, 2.0f); // 赤枠：ヒット領域

            draw->AddCircleFilled(mouseScreen, 4.0f, IM_COL32(255, 255, 0, 200)); // 黄色：実マウス

            // マウスのローカル座標（ヒット領域左上を原点）
            ImVec2 localTex = ImVec2(mouseScreen.x - contentPos.x, mouseScreen.y - contentPos.y);

            // テクスチャ上のピクセル座標に変換（重要：desc.Width/Height を使う）
            float mappedTexX = 0.0f, mappedTexY = 0.0f;
            if (contentSize.x > 0.0f && contentSize.y > 0.0f && texW > 0 && texH > 0) {
                mappedTexX = (localTex.x / io.DisplayFramebufferScale.x) * (float)texW / contentSize.x;
                mappedTexY = (localTex.y / io.DisplayFramebufferScale.y) * (float)texH / contentSize.y;
            }

            // 再び ImGui 上に戻して、マッピング位置を描画（緑十字）
            ImVec2 mappedOnImage = ImVec2(contentPos.x + (mappedTexX / (float)texW) * contentSize.x,
                contentPos.y + (mappedTexY / (float)texH) * contentSize.y);
            draw->AddLine(ImVec2(mappedOnImage.x - 8, mappedOnImage.y), ImVec2(mappedOnImage.x + 8, mappedOnImage.y), IM_COL32(0, 255, 0, 200), 2.0f);
            draw->AddLine(ImVec2(mappedOnImage.x, mappedOnImage.y - 8), ImVec2(mappedOnImage.x, mappedOnImage.y + 8), IM_COL32(0, 255, 0, 200), 2.0f);
            
            //###


        // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
        //ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);

        float centerX = winPos.x + winSize.x * 0.5f - ViewOffset.x;
        float centerY = winPos.y + winSize.y * 0.5f - ViewOffset.y;
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


        if (contentSize.x <= 0.0f || contentSize.y <= 0.0f)
        {
            contentSize.x = 1.0f;
            contentSize.y = 1.0f;
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

				// 論理ピクセルで保存
                ImVec2 localLogical = ImVec2((mouse_pos.x - contentPos.x) / io.DisplayFramebufferScale.x,
                                             (mouse_pos.y - contentPos.y) / io.DisplayFramebufferScale.y);

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
			float dragX = dragFreamebuffer.x / io.DisplayFramebufferScale.x;
			float dragY = dragFreamebuffer.y / io.DisplayFramebufferScale.y;

            ViewOffset.x -= dragX;
			ViewOffset.y -= dragY;

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測
        }

		//mouseScreen = ImGui::GetMousePos(); // 再取得
		mouseScene = ScreenToScene(mouseScreen, contentPos, ViewOffset);    // シーン座標に変換

		// ヒット領域内か -> クリックがUIのその領域内で発生したかどうかの判定に使用 スクリーン座標で判定
        bool mouseInContent = (mouseScreen.x >= contentPos.x) && (mouseScreen.x <= contentPos.x + contentSize.x) &&
			                  (mouseScreen.y >= contentPos.y) && (mouseScreen.y <= contentPos.y + contentSize.y);

		// --- クリックで選択 ---
		int clickedIndex = -1;
        if (mouseInContent && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
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

            for(size_t i=0; i<m_SceneSprites.size(); ++i)
            {
				m_SceneSprites[i].selected = ((int)i == clickedIndex);
                if (m_SceneSprites[i].selected)
                {
                    // ドラッグオフセットを記録
					m_SceneSprites[i].dragOffsetX = mouseScene.x - m_SceneSprites[i].pos_x;
					m_SceneSprites[i].dragOffsetY = mouseScene.y - m_SceneSprites[i].pos_y;
                }
            }
        }

		// --- ドラッグ移動 ---

        // ドラッグ移動は選択済みのみに
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 curMouseScene = ScreenToScene(mouseScreen, contentPos, ViewOffset);
            for (auto& s : m_SceneSprites) 
            {
                if (!s.selected) continue;
                s.pos_x = curMouseScene.x - s.dragOffsetX;
                s.pos_y = curMouseScene.y - s.dragOffsetY;
            }
        }

		// --- ドロップされたアセットを処理 ---

        for (size_t i = 0; i < m_PendingDrop.size(); i++)
        {
            const std::string& asset_name = m_PendingDrop[i];
            ImVec2 guiLocalPos = m_PendingDropPos[i];
            ImVec2 guiWindowPos = ImGui::GetWindowPos(); // シーンウィンドウの絶対位置
            
			//printf("[window_scene] Processing dropped asset: %s\n", asset_name.c_str());

            Texture* tex = n_texturemanager::instance_texmag.GetTextureName(asset_name);
            if (tex)
            {
				AddAssetToScene(tex, asset_name, guiLocalPos, guiWindowPos);    // sceneに追加
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

        }


        //printf("[window_scene/Render] m_SceneSprites.count=%zu\n", m_SceneSprites.size());

        // シーンオブジェクト一覧を走査
        for (auto& sprite : m_SceneSprites)
        {
            if (!sprite.texture) continue;

            ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(sprite.texture->tx_id);
            if (!srv) {
                printf("[window_scene/Render/ERR] sprite '%s' has null srv\n", sprite.name.c_str());
                continue;
            }


            // sprite.pos_x/pos_y はシーン内ピクセル座標（AddAssetToSceneで設定済み）
            ImVec2 screenPos = ImVec2(contentPos.x + (sprite.pos_x - ViewOffset.x), 
                                      contentPos.y + (sprite.pos_y - ViewOffset.y));

            ImGui::SetCursorScreenPos(screenPos);


            ImVec2 drawSize = ImVec2((float)sprite.width, (float)sprite.height);


			// 左クリックで選択され && ドラッグ中
            if (sprite.selected && ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
				// 再計算して現在のマウスのscene座標を取得
				ImVec2 curMouseScene = ScreenToScene(ImGui::GetMousePos(), contentPos, ViewOffset);
				sprite.pos_x = curMouseScene.x - sprite.dragOffsetX;    // ドラッグオフセットを考慮
                sprite.pos_y = curMouseScene.y - sprite.dragOffsetY;
            }

			// --- 選択された状態でEscapeキーが押されたら削除 --- 
            
			// sceneウィンドウがアクティブ && spriteが選択されている && Escapeキーが押された　時に削除
			bool isSceneWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			// 押下フレームのみ検出
            if (isSceneWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            {
				// ヒエラルキーウィンドウからも削除　後ほど実装
                for(const auto& s : m_SceneSprites)
                {
                    if (s.selected)
                    {
                        printf("[window_scene/Render] Deleting selected sprite '%s'\n", s.name.c_str());
                    }
				}
                
				// コンテナから削除
                m_SceneSprites.erase
                (
                    std::remove_if(m_SceneSprites.begin(), m_SceneSprites.end(),
                        [](const SceneSprite& s) { return s.selected; }),
                    m_SceneSprites.end()
                );

                // DBG
                printf("[window_scene/Render/DBG] Deleted selected sprites on ESC. Remaining count=%zu\n", m_SceneSprites.size());
            }

			// ### デバッグ情報表示
            //printf("[window_scene/Render/DBG] localTex=(%f,%f) mouseScene=(%f,%f) contentPos=(%f,%f) mouseScreen=(%f,%f) ViewOffset=(%f,%f) sprite.pos=(%f,%f)\n",
            //    localTex.x, localTex.y, mouseScene.x, mouseScene.y, contentPos.x, contentPos.y, mouseScreen.x, mouseScreen.y,ViewOffset.x, ViewOffset.y, sprite.pos_x, sprite.pos_y);

            //printf("[window_scene/Render/DBG] sprite='%s' pos=(%f,%f) dragOffset=(%f,%f) selected=%d\n",
            //    sprite.name.c_str(), sprite.pos_x, sprite.pos_y, sprite.dragOffsetX, sprite.dragOffsetY, sprite.selected);

            // ImGui_ImplDX11はImTextureIDにSRV ポインタを渡す実装が標準
            ImGui::Image((ImTextureID)srv, drawSize);
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

        if (!tex) { printf("[Window_scene/AddAssetToScene/ERR] tex==nullptr\n"); return; }
        //printf("[Window_scene/DBG] tex->tx_id=%p\n", tex->tx_id);

        // シーンにアセットを追加する処理をここに実装
        ImGuiIO& io = ImGui::GetIO();

        // GUI論理座標をシーン座標に変換
        ImVec2 scale = io.DisplayFramebufferScale;

        // GUI　内の絶対座標
        ImVec2 guiAbsPos = ImVec2(guiWindowPos.x + guiLocalPos.x, guiWindowPos.y + guiLocalPos.y);

		// scene座標(=論理ピクセル)
        //float sceneX = guiAbsPos.x * scale.x;
        //float sceneY = guiAbsPos.y * scale.y;
        float sceneX = guiLocalPos.x;
		float sceneY = guiLocalPos.y;

		// テクスチャのピクセルサイズを論理ピクセルに変換
        int tex_w = (float)(tex->width > 0 ? tex->width : 16) / io.DisplayFramebufferScale.x;
		int tex_h = (float)(tex->height > 0 ? tex->height : 16) / io.DisplayFramebufferScale.y;

		// 中心配置:scene座標は 左上原点なので、テクスチャの半分を引く
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

        printf("[AddAssetToScene] name=%s texPx=(%d,%d) logical=(%d,%d) pos=(%f,%f)\n",
            asset_name_str.c_str(), tex->width, tex->height, sprite.width, sprite.height, sprite.pos_x, sprite.pos_y);

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

