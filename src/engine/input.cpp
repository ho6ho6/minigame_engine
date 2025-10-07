/********************************************
 *input::Update, IsKeyDown, マウス処理の定義*
 ********************************************/

#include "../include/input.hpp"
#include <Windows.h>

/*入力*/


namespace n_input
{
	// 初期化／終了
	void Input_Start()
	{

	}

	void Input_Shutdown()
	{
		// 特に何もしない
	}

	// フレームごとに呼び出して状態更新
	void Input_Update()
	{

	}

	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}

} // namespace Input