#pragma once

#include <Windows.h>

namespace engine
{
	// �E�B���h�E�������ƏI��
	bool InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, const wchar_t* title);
	void ShutdownWindow();

	// ���C�����[�v�p
	void PollEvents();
	bool IsRunning();

	// HWND �擾�i�����W���[���A�g�p�j
	HWND GetHWND();

	bool IsRunning();

}

