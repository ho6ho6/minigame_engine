/*シーンウィンドウ*/
#include "../../include/window_editor/window_scene.hpp"
#include "../../include/render.hpp"	//フレームバッファ取得用
#include "imgui.h"					//ImGui本体
#include <stdio.h>

/* --注1--
    ImGui の API ではウィンドウ局所座標と絶対座標の概念があり、
    GetCursorScreenPos() や SetCursorScreenPos() は絶対座標、GetCursorPos() や SetCursorPos() はウィンドウ局所座標を扱うが
    これら"局所座標と絶対座標の間には現在不整合が存在"するため、絶対座標を使うのが無難。
    (2024年6月時点の ImGui 1.89.8)

    尚これはImGuiのドキュメントにも明記されていたのだ、、、
*/

// 描画されているウィンドウの座標と、内部のウィンドウの座標が異なるのは
// 局所座標と絶対座標の違いによるもの　これを修正すれば解決するだろう

/*
つまり、ImGui が描画している「シーン画像 (テクスチャ)」と
内部の「DirectX のレンダリング結果 (RenderTarget)」が別物だから　このバグが起きる
*/

static ImVec2 ViewOffset = ImVec2(0.0f, 0.0f);

namespace n_windowscene
{

    void window_scene::Render()
    {
        ImGui::SetNextWindowPos(ImVec2(920, 0), ImGuiCond_Always);
        //ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene_View");

		ImVec2 avail = ImGui::GetContentRegionAvail();  // 利用可能な領域を取得

        if (avail.x != m_LastSize.x || avail.y != m_LastSize.y) {
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

        //#
        ImVec2 mouse = ImGui::GetMousePos();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // 1) SRV (D3D11) からテクスチャサイズを取得（安全チェックを入れて）
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

        // 2) 視覚デバッグ：ヒット領域の枠とマウス位置
        ImVec2 rect_a = contentPos;
        ImVec2 rect_b = ImVec2(contentPos.x + contentSize.x, contentPos.y + contentSize.y);
        draw->AddRect(rect_a, rect_b, IM_COL32(255, 0, 0, 200), 0.0f, 0, 2.0f); // 赤枠：ヒット領域

        draw->AddCircleFilled(mouse, 4.0f, IM_COL32(255, 255, 0, 200)); // 黄色：実マウス

        // 3) マウスのローカル座標（ヒット領域左上を原点）
        ImVec2 local = ImVec2(mouse.x - contentPos.x, mouse.y - contentPos.y);

        // 4) テクスチャ上のピクセル座標に変換（重要：desc.Width/Height を使う）
        float mappedTexX = 0.0f, mappedTexY = 0.0f;
        if (contentSize.x > 0.0f && contentSize.y > 0.0f && texW > 0 && texH > 0) {
            mappedTexX = local.x * (float)texW / contentSize.x;
            mappedTexY = local.y * (float)texH / contentSize.y;
        }

        // 5) 再び ImGui 上に戻して、マッピング位置を描画（緑十字）
        ImVec2 mappedOnImage = ImVec2(contentPos.x + (mappedTexX / (float)texW) * contentSize.x,
            contentPos.y + (mappedTexY / (float)texH) * contentSize.y);
        draw->AddLine(ImVec2(mappedOnImage.x - 8, mappedOnImage.y), ImVec2(mappedOnImage.x + 8, mappedOnImage.y), IM_COL32(0, 255, 0, 200), 2.0f);
        draw->AddLine(ImVec2(mappedOnImage.x, mappedOnImage.y - 8), ImVec2(mappedOnImage.x, mappedOnImage.y + 8), IM_COL32(0, 255, 0, 200), 2.0f);

        // 6) テキスト情報（小さく右上に出す）
        char buf[256];
        snprintf(buf, sizeof(buf), "contentSize=%.0f,%.0f  tex=%d,%d  mouse=%.0f,%.0f  local=%.0f,%.0f  mappedTex=%.1f,%.1f",
            contentSize.x, contentSize.y, texW, texH, mouse.x, mouse.y, local.x, local.y, mappedTexX, mappedTexY);
        draw->AddText(ImVec2(rect_b.x - 1, rect_a.y + 2), IM_COL32(255, 255, 255, 200), buf);
        //#

        ImGui::InvisibleButton("Scene_drag_area", contentSize, ImGuiButtonFlags_MouseButtonRight);

        bool isRightActive = ImGui::IsItemActive();               // 押されている間 true

        if (isRightActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right); // 累積 delta
             // 1フレーム分に変換して反映する
            ViewOffset.x += drag.x;
            ViewOffset.y += drag.y;
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測
        }

        // シーンテクスチャをヒット領域と同じ場所に表示する
        ImGui::SetCursorScreenPos(contentPos);

        // 描画はヒット領域と同じ位置へ
        ImGui::SetCursorScreenPos(contentPos);
        ImTextureID texID = reinterpret_cast<ImTextureID>(n_render::Render_GetSceneSRV());
        ImGui::Image((void*)n_render::Render_GetSceneSRV(), ViewOffset);

        // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
        //ImDrawList* draw = ImGui::GetWindowDrawList();
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

        ImGui::End();
    }
}