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

/*ImGui は「既存の OS ウィンドウ」のクライアント領域内に GUI を即時モードで描画するライブラリ*/
// Begin/End で開くウィンドウ
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
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark(); // ImGui のテーマをダークに設定
    
    ImGui_ImplWin32_Init(window::GetHWND());
    ImGui_ImplDX11_Init(render::Render_GetDevice(), render::Render_GetDeviceContext());

    frame::Time::Start_Time();

    input::Input_Start();

    game::Game_Start();




    /*----------------------------------初期化----------------------------------*/


    /*-----------------------------ウィンドウの登録------------------------------*/

    n_windowmanager::window_manager wm;
    wm.RegisterSceneWindow();           // ImGui,DX11の初期化
	//wm.window_manager_Register<engine::editor::window_game>(); // シーンビューを登録
	//wm.window_manager_Register<engine::editor::window_input>(); // シーンビューを登録


    /*-----------------------------ウィンドウの登録------------------------------*/


    /*----------------------------------メイン----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {
        // 1) OSメッセージ処理 は必ず NewFrame より前
        window::PollEvents();

        // 2) 各サブシステム更新
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();
        input::Input_Update();
        game::Game_Update(deltaTime);

        // 3) ImGui フレーム開始
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 4) ImGui ウィンドウ群の描画
        //    まずデバッグ用にデモウィンドウを出してみる
        //ImGui::ShowDemoWindow();

        //    あなたの Scene ウィンドウ登録・描画
        wm.RenderAll();

        // 5) ImGui 描画コマンドを確定
        ImGui::Render();

        // 6) レンダラーのバッファクリア＋ゲームオブジェクト描画
        //    （Render_Update がここで背景クリア→Game_Renderもしくは
        //     swapChain->Present を行っていない場合は、Begin/Endを整形してください）
        FLOAT clear_color[4] = { 0.1f,0.1f,0.1f,1.0f };

        // game::Game_Render();                        // ← 高レベル描画
        // 7) ImGui 描画をバックバッファへ上書き
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        // 8) 最後に Present
        render::Render_Present();                   // ← swapChain->Present(1,0)

        render::Render_Update(deltaTime, frameTime);
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
