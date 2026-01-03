#ifndef RENDER_H
#define RENDER_H

#include <windows.h>
#include <cstdint>
#include <d3d11.h>

#include <imgui.h>
#include <imgui_internal.h> // ImGuiWindow型の定義

#include <directXMath.h>
using namespace DirectX;

struct CBChangesEveryFrame
{
	XMMATRIX View;			// XMMatrixTranspose()する
	XMMATRIX Projection;
};

namespace n_render
{

	bool Render_Start(HWND hwnd, int width, int height);    // GDI+ 初期化
	void Render_Shutdown();                                    // GDI+ 終了処理

	//   clearColor: {r, g, b, a}
	void Render_Frame(const float clearColor[4]);

	// 描画コマンド確定（Present は行わない）
	void Render_EndFrame();

	// SwapChain→Present
	void Render_Present();

	// win_w, win_h: ウィンドウのクライアント領域のサイズ
	void Render_Resizeviewport(int win_w, int win_h, int fb_w, int fb_h);

	// 画面 (x, y) からオブジェクトをピックし、ID を返す
	// -1 を返したら「何も選択されていない」
	int Render_PickObject(int x, int y);

	// シーン描画用テクスチャを作成する
	bool Render_CreateSceneTexture(int width, int height);

	/* DirectX11 のCOMインターフェイスへのポインタ */

	// ビューポートやレンダーターゲットの内容をシェーダーがサンプルできる形（Shader Resource View）で取得
	// 現在の画面をテクスチャとして参照するためのSRVを返す
	ID3D11ShaderResourceView* Render_GetSceneSRV();

	/* DirectX11 のデバイスとコンテキストを取得 */

	// Direct3D の デバイスオブジェクト
	// GPUにリソース(テクスチャ/バッファ/シェーダーなどを)を作成・管理するためのインターフェイス
	ID3D11Device* Render_GetDevice();

	// 描画コマンドやパイプライン状態の設定、リソースのバインドを行う コンテキスト（即時コンテキスト）
	// 実際に描画命令をGPUに送るためのインターフェイス
	ID3D11DeviceContext* Render_GetDeviceContext();

	// レンダリング結果を書き込む レンダーターゲットのビュー
	// 画面に描画するためのターゲットを表すインターフェイス
	ID3D11RenderTargetView* Render_GetRenderTargetView();

}

#endif 