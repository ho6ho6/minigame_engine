/***********************************************************
 *  Win32�@�E�B���h�E�����E�C�x���g�����E�o�b�t�@�Ǘ��̐錾*
 ***********************************************************/


#pragma once

#include <Windows.h>

namespace window
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

