/***********************************************************************************
 * WinMain �ƃ��C�����[�v�̌Ăяo���A�e�T�u�V�X�e���̏������ƏI���������s��        *
 ***********************************************************************************/

#include "include/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "include/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "include/game.hpp"      // Game_Start(), Game_Update(), Game_Render(), Game_Shutdown()
#include "include/time.hpp"      // Time::Start(), Time::Update(), Time::GetDeltaTime()
#include "include/render.hpp"    // InitRenderer(), BeginFrame(), RenderFrame(), EndFrame(), ShutdownRenderer()

#include <Windows.h>
#include <iostream>

/*
  WinMain �̈���:
    HINSTANCE   hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��
    HINSTANCE   hPrevInstance : ��� NULL
    LPSTR       lpCmdLine     : �R�}���h���C�������iANSI�j
    int         nCmdShow      : �E�B���h�E�̏����\�����@
*/

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
    if (!window::InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine"))
        return -1;

	//2. render,input,time�̏�����
	if (!render::Render_Start(window::GetHWND(), width, height)) // render.cpp
        return -1;

    input::Input_Start(); // input.cpp

	if (!game::Game_Start()) // game.cpp
		return -1;

    //3. �^�C��������,�����_�[������
    frame::Time::Start_Time();

    //�Q�[�����W�b�N������
	game::Game_Start(); // game.cpp �Q�[���S�̂̏�����


    // 4. ���C�����[�v
    while (window::IsRunning()) //window.cpp
    {
        //time�X�V    time.cpp
        float deltaTime = frame::Time::Update_Time();
		uint64_t frameTime = frame::Time::GetFrameCount();

        // �C�x���g�|�[�����O window.cpp
        window::PollEvents();
        
        // �T�u�V�X�e���X�V
		input::Input_Update();  // input.cpp 
        game::Game_Update(deltaTime); // game.cpp ���W�b�N�X�V

        // �`��
		game::Game_Render(); // game.cpp    �����x���`��i�I�u�W�F�N�g�P�ʁj
        render::Render_Update(deltaTime, frameTime); // render.cpp  �჌�x���`��i�w�i�N���A�{�S�I�u�W�F�N�g���܂Ƃ߂ďo�́j

    }

    // 5. �I������
    input::Input_Shutdown();             // input.cpp
    render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}
