/********************************************
 *input::Update, IsKeyDown, �}�E�X�����̒�`*
 ********************************************/

#include "engine/input.hpp"

/*����*/

static std::array<BYTE, 256> gPrevKeyState{};
static std::array<BYTE, 256> gCurrKeyState{};

namespace input
{
	// �������^�I��
	void Input_Start()
	{
		// �L�[��Ԃ�������
		gPrevKeyState.fill(0);
		gCurrKeyState.fill(0);
	}

	void Input_Shutdown()
	{
		// ���ɉ������Ȃ�
	}

	// �t���[�����ƂɌĂяo���ď�ԍX�V
	void Input_Update()
	{
		// �O�t���[���̏�Ԃ��R�s�[
		gPrevKeyState = gCurrKeyState;

		// ���݂̃L�[��Ԃ��擾
		for (int i = 0; i < 256; ++i)
		{
			// ��ʃr�b�g�i0x8000�j�������Ă���Ή�����Ă���
			gCurrKeyState[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
		}
	}

	// ��ԃN�G��
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