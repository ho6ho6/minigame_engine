/********************************************
 *input::Update, IsKeyDown, �}�E�X�����̒�`*
 ********************************************/

#include "../include/input.hpp"
#include <Windows.h>

/*����*/


namespace n_input
{
	// �������^�I��
	void Input_Start()
	{

	}

	void Input_Shutdown()
	{
		// ���ɉ������Ȃ�
	}

	// �t���[�����ƂɌĂяo���ď�ԍX�V
	void Input_Update()
	{

	}

	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}

} // namespace Input