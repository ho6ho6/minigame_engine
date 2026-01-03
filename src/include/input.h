#ifndef INPUT_H
#define INPUT_H

#include <array>
#include <windows.h>
#include "imgui.h"

/*入力を受け取る*/

// UI 側で使う小さなヘルパー 
struct KeyBindState
{ 
	bool listening = false; 
	ImGuiKey* targetKey = nullptr; // 変更対象ポインタ
};

namespace n_input
{
	void PollPlayerInputAndEnqueue(int64_t eid);
}

#endif // INPUT_H