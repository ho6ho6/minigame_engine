/************************************************************
 *キーボード・マウス状態の収集・問い合わせインタフェース宣言*
 ************************************************************/

#pragma once
#include <array>
#include <windows.h>

/*入力を受け取る*/

namespace Input
{
    // 初期化／終了
    void InitInput();
    void ShutdownInput();

    // フレームごとに呼び出して状態更新
    void ProcessInput();

    // 状態クエリ
    bool IsKeyDown(int vk);      // 押しっぱなし
    bool IsKeyPressed(int vk);   // 押した瞬間
    bool IsKeyReleased(int vk);  // 離した瞬間
}