/**************************************************************
*シーンデータを受け取り、描画コマンドを発行するインタフェース			    *
**************************************************************/

#ifndef RENDER_HPP
#define RENDER_HPP


#pragma once
#include <windows.h>
#include <cstdint>
#include <d3d11.h>

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
	void Render_Frame(const float clearColor[4], float dt, uint64_t frameCount);

	// 描画コマンド確定（Present は行わない）
	void Render_EndFrame();

	// SwapChain→Present
	void Render_Present();

	// ビューポートのサイズ変更
	void Render_Resizeviewport(int width, int height);

	// 画面 (x, y) からオブジェクトをピックし、ID を返す
    // -1 を返したら「何も選択されていない」
	int Render_PickObject(int x, int y);

	ID3D11ShaderResourceView* Render_GetSceneSRV(); // ビューポートのテクスチャを取得
	// DirectX 11 のデバイスとコンテキストを取得
	ID3D11Device*				Render_GetDevice();
	ID3D11DeviceContext*		Render_GetDeviceContext();
	ID3D11RenderTargetView* Render_GetRenderTargetView();

}

#endif // RENDER_HPP