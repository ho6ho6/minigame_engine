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

/*ImGui �́u������ OS �E�B���h�E�v�̃N���C�A���g�̈���� GUI �𑦎����[�h�ŕ`�悷�郉�C�u����*/
// Begin/End �ŊJ���E�B���h�E
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
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark(); // ImGui �̃e�[�}���_�[�N�ɐݒ�
    
    ImGui_ImplWin32_Init(window::GetHWND());
    ImGui_ImplDX11_Init(render::Render_GetDevice(), render::Render_GetDeviceContext());

    frame::Time::Start_Time();

    input::Input_Start();

    game::Game_Start();




    /*----------------------------------������----------------------------------*/


    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/

    n_windowmanager::window_manager wm;
    wm.RegisterSceneWindow();           // ImGui,DX11�̏�����
	//wm.window_manager_Register<engine::editor::window_game>(); // �V�[���r���[��o�^
	//wm.window_manager_Register<engine::editor::window_input>(); // �V�[���r���[��o�^


    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/


    /*----------------------------------���C��----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {
        // 1) OS���b�Z�[�W���� �͕K�� NewFrame ���O
        window::PollEvents();

        // 2) �e�T�u�V�X�e���X�V
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();
        input::Input_Update();
        game::Game_Update(deltaTime);

        // 3) ImGui �t���[���J�n
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 4) ImGui �E�B���h�E�Q�̕`��
        //    �܂��f�o�b�O�p�Ƀf���E�B���h�E���o���Ă݂�
        //ImGui::ShowDemoWindow();

        //    ���Ȃ��� Scene �E�B���h�E�o�^�E�`��
        wm.RenderAll();

        // 5) ImGui �`��R�}���h���m��
        ImGui::Render();

        // 6) �����_���[�̃o�b�t�@�N���A�{�Q�[���I�u�W�F�N�g�`��
        //    �iRender_Update �������Ŕw�i�N���A��Game_Render��������
        //     swapChain->Present ���s���Ă��Ȃ��ꍇ�́ABegin/End�𐮌`���Ă��������j
        FLOAT clear_color[4] = { 0.1f,0.1f,0.1f,1.0f };

        // game::Game_Render();                        // �� �����x���`��
        // 7) ImGui �`����o�b�N�o�b�t�@�֏㏑��
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        // 8) �Ō�� Present
        render::Render_Present();                   // �� swapChain->Present(1,0)

        render::Render_Update(deltaTime, frameTime);
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
