#include "input.hpp"
#include "renndert.hpp"

/*ゲームエンジンの中心*/

int main()
{
    InitWindow();       // ウィンドウ初期化（Win32 APIなど）
    InitInput();        // 入力初期化
    InitRenderer();     // 描画初期化

    while (IsRunning()) {
        ProcessInput(); // 入力処理
        Update();       // ゲーム状態更新（後で追加）
        Render();       // 描画処理
    }

    Shutdown();         // 終了処理
    return 0;
}