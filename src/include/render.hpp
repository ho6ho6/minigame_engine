/**************************************************************
*シーンデータを受け取り、描画コマンドを発行するインタフェース			    *
**************************************************************/

#ifndef RENDER_HPP
#define RENDER_HPP


#pragma once
#include <windows.h>
#include <cstdint>
#include <d3d11.h>

namespace render
{
	bool Render_Start(HWND hwnd, int width, int height);    // GDI+ 初期化
	void Render_Shutdown();                                    // GDI+ 終了処理

    // ここに追加：毎フレーム呼び出す Render 関数
    void Render_Update(float , uint64_t );

	//   clearColor: {r, g, b, a}
	void Render_BegineFrame(const float clearColor[4]);

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

}

#endif // RENDER_HPP