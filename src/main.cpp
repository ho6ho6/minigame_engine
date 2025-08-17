/***********************************************************************************
 * WinMain とメインループの呼び出し、各サブシステムの初期化と終了処理を行う        *
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
  WinMain の引数:
    HINSTANCE   hInstance     : このアプリケーションのインスタンスハンドル
    HINSTANCE   hPrevInstance : 常に NULL
    LPSTR       lpCmdLine     : コマンドライン引数（ANSI）
    int         nCmdShow      : ウィンドウの初期表示方法
*/

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{

    /*----------------------------------初期化----------------------------------*/

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

    /*----------------------------------初期化----------------------------------*/


    /*-----------------------------ウィンドウの登録------------------------------*/

    engine::window::window_manager wm;
	wm.window_manager_Register<engine::editor::window_scene>(); // シーンビューを登録
	//wm.window_manager_Register<engine::editor::window_game>(); // シーンビューを登録
	//wm.window_manager_Register<engine::editor::window_input>(); // シーンビューを登録

    /*-----------------------------ウィンドウの登録------------------------------*/


    /*----------------------------------メイン----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {

        // イベントポーリング window.cpp
        window::PollEvents();

        //time更新    time.cpp
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();


        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // サブシステム更新
		input::Input_Update();  // input.cpp 
        game::Game_Update(deltaTime); // game.cpp ロジック更新

        // 描画
		game::Game_Render(); // game.cpp    高レベル描画（オブジェクト単位）
        render::Render_Update(deltaTime, frameTime); // render.cpp  低レベル描画（背景クリア＋全オブジェクトをまとめて出力）

        // ImGui::ShowDemoWindow();

		wm.window_manager_RenderAll(); // window_manager.cpp

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // ImGuiの描画を行うために、DirectX11のレンダリングを開始
    }

    /*----------------------------------メイン----------------------------------*/


    // 5. 終了処理
	ImGui_ImplDX11_Shutdown();        // ImGuiのDirectX11レンダリングを終了
	ImGui_ImplWin32_Shutdown();       // ImGuiのWin32プラットフォームを終了
	ImGui::DestroyContext();            // ImGuiコンテキストを破棄

    input::Input_Shutdown();             // input.cpp
    render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}
