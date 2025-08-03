/***********************************************************************************
 * WinMain �ƃ��C�����[�v�̌Ăяo���A�e�T�u�V�X�e���̏������ƏI���������s��        *
 ***********************************************************************************/

#include "engine/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "engine/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "engine/game.hpp"      //

#include <Windows.h>

/*
  WinMain �̈���:
    HINSTANCE   hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��
    HINSTANCE   hPrevInstance : ��� NULL
    LPSTR       lpCmdLine     : �R�}���h���C�������iANSI�j
    int         nCmdShow      : �E�B���h�E�̏����\�����@
*/

using namespace window;

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    // 1. �E�B���h�E������
    const int width = 1920;
    const int height = 1080;
    if (!InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine"))
        return -1;

    // 2. ���C�����[�v
    while (IsRunning()) //window.cpp
    {
		PollEvents(); // �C�x���g�|�[�����O
        // ������ Update() �� Render() ���ォ��ǉ�
    }

    // 3. �I������
    ShutdownWindow();
    return 0;
}