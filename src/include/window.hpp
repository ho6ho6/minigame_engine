/***********************************************************
 *  Win32�@�E�B���h�E�����E�C�x���g�����E�o�b�t�@�Ǘ��̐錾*
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
        // �K�v�Ȃ� resize �C�x���g����
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


namespace window
{
	// �E�B���h�E�������ƏI��
	bool InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title);
	void ShutdownWindow();

	// ���C�����[�v�p
	void PollEvents();
	bool IsRunning();

	// HWND �擾�i�����W���[���A�g�p�j
	HWND GetHWND();

}

#endif // WINDOW_HPP