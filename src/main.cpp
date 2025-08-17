/***********************************************************************************
 * WinMain �ƃ��C�����[�v�̌Ăяo���A�e�T�u�V�X�e���̏������ƏI���������s��        *
 ***********************************************************************************/

#include "include/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "include/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "include/game.hpp"      // Game_Start(), Game_Update(), Game_Render(), Game_Shutdown()
#include "include/time.hpp"      // Time::Start(), Time::Update(), Time::GetDeltaTime()
#include "include/render.hpp"    // InitRenderer(), BeginFrame(), RenderFrame(), EndFrame(), ShutdownRenderer()

#include "include/window_editor/window_manager.hpp" // window_manager
#include "include/window_editor/window_scene.hpp" // window_scene
#include "include/window_editor/window_editor.hpp" // window_editor
#include "include/window_editor/window_manager_abstra.hpp" // window_manager_abst
#include "include/window_editor/window_manager_other.hpp" // window_manager_other

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

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

    /*----------------------------------������----------------------------------*/

    const int width = 1920;
    const int height = 1080;

    if (!window::InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine")) return -1;

	if (!game::Game_Start()) return -1;

    if (!render::Render_Start(window::GetHWND(), width, height)) return -1;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(window::GetHWND());
    ImGui_ImplDX11_Init(render::Render_GetDevice(), render::Render_GetDeviceContext());

    frame::Time::Start_Time();

    input::Input_Start();

    game::Game_Start();

    /*----------------------------------������----------------------------------*/


    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/

    engine::window::window_manager wm;
	wm.window_manager_Register<engine::editor::window_scene>(); // �V�[���r���[��o�^
	//wm.window_manager_Register<engine::editor::window_game>(); // �V�[���r���[��o�^
	//wm.window_manager_Register<engine::editor::window_input>(); // �V�[���r���[��o�^

    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/


    /*----------------------------------���C��----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {

        // �C�x���g�|�[�����O window.cpp
        window::PollEvents();

        //time�X�V    time.cpp
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();


        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // �T�u�V�X�e���X�V
		input::Input_Update();  // input.cpp 
        game::Game_Update(deltaTime); // game.cpp ���W�b�N�X�V

        // �`��
		game::Game_Render(); // game.cpp    �����x���`��i�I�u�W�F�N�g�P�ʁj
        render::Render_Update(deltaTime, frameTime); // render.cpp  �჌�x���`��i�w�i�N���A�{�S�I�u�W�F�N�g���܂Ƃ߂ďo�́j

        // ImGui::ShowDemoWindow();

		wm.window_manager_RenderAll(); // window_manager.cpp

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // ImGui�̕`����s�����߂ɁADirectX11�̃����_�����O���J�n
    }

    /*----------------------------------���C��----------------------------------*/


    // 5. �I������
	ImGui_ImplDX11_Shutdown();        // ImGui��DirectX11�����_�����O���I��
	ImGui_ImplWin32_Shutdown();       // ImGui��Win32�v���b�g�t�H�[�����I��
	ImGui::DestroyContext();            // ImGui�R���e�L�X�g��j��

    input::Input_Shutdown();             // input.cpp
    render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}
