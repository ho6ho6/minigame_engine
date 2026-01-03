#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>


namespace n_window
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