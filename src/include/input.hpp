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
	void UpdateInputAndJumpForAll();
	void PhysicsStep(float dt);
}

#endif // INPUT_HPP