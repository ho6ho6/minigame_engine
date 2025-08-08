/**************************************************************
*シーンデータを受け取り、描画コマンドを発行するインタフェース			    *
**************************************************************/

#ifndef RENDER_HPP
#define RENDER_HPP


#pragma once
#include <windows.h>
#include <cstdint>

namespace render
{
	bool Render_Start(HWND hwnd, int width, int height);    // GDI+ 初期化
	void Render_Shutdown();                                    // GDI+ 終了処理

    // ここに追加：毎フレーム呼び出す Render 関数
    void Render_Update(float , uint64_t );
}

#endif // RENDER_HPP