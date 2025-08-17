/***********************************************************
 *  Win32　ウィンドウ生成・イベント処理・バッファ管理の宣言*
 ***********************************************************/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma once
#include <Windows.h>

static HWND g_hWnd = nullptr;
static HINSTANCE g_hInstance = nullptr;
static bool g_isRunning = false;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        g_isRunning = false;
        return 0;
    case WM_SIZE:
        // 必要なら resize イベント処理
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


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