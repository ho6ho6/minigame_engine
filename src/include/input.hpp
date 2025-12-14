/************************************************************
 *キーボード・マウス状態の収集・問い合わせインタフェース宣言*
 ************************************************************/

#ifndef INPUT_HPP
#define INPUT_HPP

#pragma once
#include <array>
#include <windows.h>

 /*入力を受け取る*/

namespace n_input
{
    // 初期化／終了
    void Input_Start();
    void Input_Shutdown();

    // フレームごとに呼び出して状態更新
    void Input_Update();

    // 状態クエリ
    bool IsKeyDown(int key);      // 押しっぱなし
    bool IsKeyPressed(int key);   // 押した瞬間
    bool IsKeyReleased(int key);  // 離した瞬間
}

#endif // INPUT_HPP