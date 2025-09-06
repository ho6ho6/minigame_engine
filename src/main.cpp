/**************************************************************************
 * WinMain �ƃ��C�����[�v�̌Ăяo���A�e�T�u�V�X�e���̏������ƏI���������s��        *
 **************************************************************************/

#include "include/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "include/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "include/game.hpp"      // Game_Start(), Game_Update(), Game_Render(), Game_Shutdown()
#include "include/time.hpp"      // Time::Start(), Time::Update(), Time::GetDeltaTime()
#include "include/render.hpp"    // InitRenderer(), BeginFrame(), RenderFrame(), EndFrame(), ShutdownRenderer()

#include "include/window_editor/window_manager.hpp" // window_manager
#include "include/assets/assets_manager/texture_manager.hpp" // texture_manager
#include "include/window_editor/window_assets.hpp"
#include "include/assets/assets_manager/assets_manager.hpp"
#include "include/assets/util.hpp"

/*ImGui �́u������ OS �E�B���h�E�v�̃N���C�A���g�̈���� GUI �𑦎����[�h�ŕ`�悷�郉�C�u����*/
// Begin/End �ŊJ���E�B���h�E
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <Windows.h>
#include <iostream>
#include <filesystem>

/*
  WinMain �̈���:
    HINSTANCE   hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��
    HINSTANCE   hPrevInstance : ��� NULL
    LPSTR       lpCmdLine     : �R�}���h���C�������iANSI�j
    int         nCmdShow      : �E�B���h�E�̏����\�����@
*/

n_texturemanager::texture_manager g_TextureManager("../../Assets/textures"); // �O���[�o���ȃe�N�X�`���}�l�[�W��
n_assetsmanager::assets_manager g_AssetsManager(g_TextureManager); // �O���[�o���ȃA�Z�b�g�}�l�[�W��

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

    if (!n_render::Render_Start(window::GetHWND(), width, height)) return -1;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark(); // ImGui �̃e�[�}���_�[�N�ɐݒ�

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Docking�@�\��ON
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // �i�C�Ӂj�ʃE�B���h�E�ւ̓W�J���\��

    
    ImGui_ImplWin32_Init(window::GetHWND());
    ImGui_ImplDX11_Init(n_render::Render_GetDevice(), n_render::Render_GetDeviceContext());

    frame::Time::Start_Time();

    //input::Input_Start();

    //game::Game_Start();




    /*----------------------------------������----------------------------------*/


    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/

    n_windowmanager::window_manager wm;
    wm.Register_SceneWindow();           // ImGui,DX11�̏�����
    wm.Register_GameWindow();
    //wm.Register_InputWindow();    //�ʃE�B���h�E�𐶐����đ��삷��\��������B
    wm.Register_Hierarchywindow();
    wm.Register_Assetswindow();


    /*-----------------------------�E�B���h�E�̓o�^------------------------------*/


	/*------------------------------Assets�̓o�^-------------------------------*/

    // �e�N�X�`����ǂݍ���
    g_TextureManager.LoadAllTextures();

    /*------------------------------Assets�̓o�^-------------------------------*/


    /*----------------------------------���C��----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {
        window::PollEvents();

        // 2) �e�T�u�V�X�e���X�V
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();
        //input::Input_Update();
        //game::Game_Update(deltaTime);


        // 3) ImGui �t���[���J�n
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //Assets �e�N�X�`���̕`��
		g_AssetsManager.assets_Show();

        static const float clear_col[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
        n_render::Render_Frame(clear_col, deltaTime, frameTime);

        // ���� �������� DockSpace ����
        // �z�X�g�p�t���X�N���[���E�B���h�E�𗧂Ă�
        ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("##MainDockHost", nullptr, hostFlags);

        // DockSpace �{��
        ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(
            dockspaceID, ImVec2(0, 0),
            ImGuiDockNodeFlags_PassthruCentralNode
        );
        ImGui::End();

        wm.RenderAll();

        // ImGui �`��m��
        ImGui::Render();
        
        auto ctx = n_render::Render_GetDeviceContext();
        auto rtv = n_render::Render_GetRenderTargetView();

        ctx->OMSetRenderTargets(1, &rtv, nullptr);
        ctx->ClearRenderTargetView(rtv, clear_col);

        //game::Game_Render(); // game.cpp
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // 6) �Ō�� DirectX �� Present
        n_render::Render_Present();
    }

    /*----------------------------------���C��----------------------------------*/


    // 5. �I������
	ImGui_ImplDX11_Shutdown();        // ImGui��DirectX11�����_�����O���I��
	ImGui_ImplWin32_Shutdown();       // ImGui��Win32�v���b�g�t�H�[�����I��
	ImGui::DestroyContext();            // ImGui�R���e�L�X�g��j��

    input::Input_Shutdown();             // input.cpp
    n_render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}