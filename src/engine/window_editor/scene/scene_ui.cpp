#include "include/render.h"
#include "include/window_editor/scene/scene_ui.h"

namespace n_windowscene::ui
{
    // ui表示
	void SetupScene()
	{
		ImGui::SetNextWindowPos(ImVec2(920, 0), ImGuiCond_Always);

		ImGui::Begin("Scene_View", nullptr, ImGuiWindowFlags_NoResize);

		// window サイズを固定
		ImGui::SetWindowSize(ImVec2(1000, 600));
	}

    // Sceneに描画されるグリッド描画
    void DrawSceneGrid(const SceneViewContext& ctx, ImDrawList* draw)
    {

        // --- 以下は座標系／グリッド描画（クリップや origin 計算はそのまま） ---
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        draw->PushClipRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), true);

        float centerX = winPos.x + winSize.x * 0.5f - ctx.camera->viewOffset.x;
        float centerY = winPos.y + winSize.y * 0.5f - ctx.camera->viewOffset.y;
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

    // window_sceneの当たり判定デバック
	void DebugSceneFrame(const SceneViewContext& ctx, ImDrawList* draw)
	{
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
            mappedTexX = (localTex.x / ctx.content.fbScale.x) * (float)texW / ctx.content.screenPos.x;
            mappedTexY = (localTex.y / ctx.content.fbScale.y) * (float)texH / ctx.content.screenPos.y;
        }

        // 再び ImGui 上に戻して、マッピング位置を描画（緑十字）
        ImVec2 mappedOnImage = ImVec2(ctx.content.screenPos.x + (mappedTexX / (float)texW) * ctx.content.screenPos.x,
            ctx.content.size.y + (mappedTexY / (float)texH) * ctx.content.size.y);
        draw->AddLine(ImVec2(mappedOnImage.x - 8, mappedOnImage.y), ImVec2(mappedOnImage.x + 8, mappedOnImage.y), IM_COL32(0, 255, 0, 200), 2.0f);
        draw->AddLine(ImVec2(mappedOnImage.x, mappedOnImage.y - 8), ImVec2(mappedOnImage.x, mappedOnImage.y + 8), IM_COL32(0, 255, 0, 200), 2.0f);

        //###
	}
}