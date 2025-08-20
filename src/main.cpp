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

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Docking�@�\��ON
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // �i�C�Ӂj�ʃE�B���h�E�ւ̓W�J���\��

    
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
        window::PollEvents();

        // 2) �e�T�u�V�X�e���X�V
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();
        input::Input_Update();
        game::Game_Update(deltaTime);

        static const float clear_col[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        render::Render_BegineFrame(clear_col);

        // 3) ImGui �t���[���J�n
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ���� �������� DockSpace ����
        // (A) �z�X�g�p�t���X�N���[���E�B���h�E�𗧂Ă�
        ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("##MainDockHost", nullptr, hostFlags);

        // (B) DockSpace �{��
        ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(
            dockspaceID, ImVec2(0, 0),
            ImGuiDockNodeFlags_PassthruCentralNode
        );



		// �f�t�H���g�� DockSpace ���쐬����ꍇ�͈ȉ��̂悤�ɃR�����g�A�E�g���O��
        //static bool firstTime = true;
        //if (firstTime)
        //{
        //    firstTime = false;
        //    ImGuiID dockMain = dockspaceID;
        //    ImGui::DockBuilderRemoveNode(dockMain);
        //    ImGui::DockBuilderAddNode(dockMain, ImGuiDockNodeFlags_DockSpace);
        //    ImGui::DockBuilderSetNodeSize(dockMain, ImGui::GetIO().DisplaySize);

        //    // ���Ƀq�G�����L�[�A�E�ɃC���X�y�N�^�p�ɏc����
        //    ImGuiID leftID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
        //    ImGuiID rightID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.2f, nullptr, &dockMain);

        //    // �����ɃR���\�[���p�̉�����
        //    ImGuiID bottomID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain);

        //    // �p�l�������ꂼ��z�u
        //    ImGui::DockBuilderDockWindow("Hierarchy", leftID);
        //    ImGui::DockBuilderDockWindow("Inspector", rightID);
        //    ImGui::DockBuilderDockWindow("Console", bottomID);
        //    ImGui::DockBuilderDockWindow("Scene", dockMain);

        //    ImGui::DockBuilderFinish(dockMain);
        //}

        ImGui::End();
        // ���� DockSpace �ݒu�����܂� ����

        // 4) �e�p�l����`�悷��
        //    ��Begin()/End() �̃^�C�g���� "Hierarchy" �Ȃǈ�v���Ă��邱��
        //DrawHierarchy();   // ��: void DrawHierarchy() { ImGui::Begin("Hierarchy"); �c ImGui::End(); }
        //DrawInspector();   // ��:
        //wm.RenderAll();    // Scene View (window_manager�o�R)
        //DrawConsole();     // ��:

        // 5) ImGui �`��m��
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // 6) �Ō�� DirectX �� Present
        render::Render_Present();
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

//void DrawHierarchy()
//{
//    ImGui::Begin("Hierarchy");
//    // �c���[�\���Ȃ�
//    ImGui::Text("GameObject A");
//    ImGui::Text("GameObject B");
//    ImGui::End();
//}
//
//void DrawInspector()
//{
//    ImGui::Begin("Inspector");
//    // �I�𒆃I�u�W�F�N�g�̃v���p�e�B
//    ImGui::Text("Position");
//    ImGui::DragFloat3("##pos", glm::value_ptr(selectedPos));
//    ImGui::End();
//}
//
//void DrawScene()
//{
//    ImGui::Begin("Scene");
//    // �V�[���g���[�X�A�܂���DirectX�̃V�F�A�e�N�X�`���Ȃǂ�`��
//    ImGui::Image((void*)sceneTextureSRV, ImGui::GetContentRegionAvail());
//    ImGui::End();
//}
//
//void DrawConsole()
//{
//    ImGui::Begin("Console");
//    // ���O�o��
//    for (auto& msg : logBuffer)
//        ImGui::TextUnformatted(msg.c_str());
//    ImGui::End();
//}