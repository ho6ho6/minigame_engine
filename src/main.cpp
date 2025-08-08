/***********************************************************************************
 * WinMain とメインループの呼び出し、各サブシステムの初期化と終了処理を行う        *
 ***********************************************************************************/

#include "include/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "include/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "include/game.hpp"      // Game_Start(), Game_Update(), Game_Render(), Game_Shutdown()
#include "include/time.hpp"      // Time::Start(), Time::Update(), Time::GetDeltaTime()
#include "include/render.hpp"    // InitRenderer(), BeginFrame(), RenderFrame(), EndFrame(), ShutdownRenderer()

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
    // 1. ウィンドウ初期化
    const int width = 1920;
    const int height = 1080;
    if (!window::InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine"))
        return -1;

	//2. render,input,timeの初期化
	if (!render::Render_Start(window::GetHWND(), width, height)) // render.cpp
        return -1;

    input::Input_Start(); // input.cpp

	if (!game::Game_Start()) // game.cpp
		return -1;

    //3. タイム初期化,レンダー初期化
    frame::Time::Start_Time();

    //ゲームロジック初期化
	game::Game_Start(); // game.cpp ゲーム全体の初期化


    // 4. メインループ
    while (window::IsRunning()) //window.cpp
    {
        //time更新    time.cpp
        float deltaTime = frame::Time::Update_Time();
		uint64_t frameTime = frame::Time::GetFrameCount();

        // イベントポーリング window.cpp
        window::PollEvents();
        
        // サブシステム更新
		input::Input_Update();  // input.cpp 
        game::Game_Update(deltaTime); // game.cpp ロジック更新

        // 描画
		game::Game_Render(); // game.cpp    高レベル描画（オブジェクト単位）
        render::Render_Update(deltaTime, frameTime); // render.cpp  低レベル描画（背景クリア＋全オブジェクトをまとめて出力）

    }

    // 5. 終了処理
    input::Input_Shutdown();             // input.cpp
    render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}
