/********************************************
 *input::Update, IsKeyDown, マウス処理の定義*
 ********************************************/

#include "engine/input.hpp"

/*入力*/

static std::array<BYTE, 256> gPrevKeyState{};
static std::array<BYTE, 256> gCurrKeyState{};

namespace input
{
	// 初期化／終了
	void Input_Start()
	{
		// キー状態を初期化
		gPrevKeyState.fill(0);
		gCurrKeyState.fill(0);
	}

	void Input_Shutdown()
	{
		// 特に何もしない
	}

	// フレームごとに呼び出して状態更新
	void Input_Update()
	{
		// 前フレームの状態をコピー
		gPrevKeyState = gCurrKeyState;

		// 現在のキー状態を取得
		for (int i = 0; i < 256; ++i)
		{
			// 上位ビット（0x8000）が立っていれば押されている
			gCurrKeyState[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
		}
	}

	// 状態クエリ
	bool IsKeyDown(int vk)
	{
		return (gCurrKeyState[vk] & 0x80) != 0;
	}

	bool IsKeyPressed(int vk)
	{
		return (gCurrKeyState[vk] & 0x80) != 0 && (gPrevKeyState[vk] & 0x80) == 0;
	}

	bool IsKeyReleased(int vk)
	{
		return (gCurrKeyState[vk] & 0x80) == 0 && (gPrevKeyState[vk] & 0x80) != 0;
	}
} // namespace Input