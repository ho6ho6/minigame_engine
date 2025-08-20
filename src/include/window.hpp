/***********************************************************
 *  Win32　ウィンドウ生成・イベント処理・バッファ管理の宣言*
 ***********************************************************/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma once
#include <Windows.h>


namespace window
{
	// ウィンドウ初期化と終了
	bool InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title);
	void ShutdownWindow();

	// メインループ用
	void PollEvents();
	bool IsRunning();

	// HWND 取得（他モジュール連携用）
	HWND GetHWND();

}

#endif // WINDOW_HPP