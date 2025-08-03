/***********************************************************************************
 * WinMain とメインループの呼び出し、各サブシステムの初期化と終了処理を行う        *
 ***********************************************************************************/

#include "engine/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "engine/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "engine/game.hpp"      //

#include <Windows.h>

/*
  WinMain の引数:
    HINSTANCE   hInstance     : このアプリケーションのインスタンスハンドル
    HINSTANCE   hPrevInstance : 常に NULL
    LPSTR       lpCmdLine     : コマンドライン引数（ANSI）
    int         nCmdShow      : ウィンドウの初期表示方法
*/

using namespace window;

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
    if (!InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine"))
        return -1;

    // 2. メインループ
    while (IsRunning()) //window.cpp
    {
		PollEvents(); // イベントポーリング
        // ここに Update() や Render() を後から追加
    }

    // 3. 終了処理
    ShutdownWindow();
    return 0;
}