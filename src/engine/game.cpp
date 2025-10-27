/************************************************************
 *�Q�[���S�̂̏�ԊǗ��E�V�[�������E�X�e�[�g�}�V���Ȃǂ�錾			*
 ************************************************************/

#include "include/game.hpp"
#include "include/input.hpp"
#include "include/render.hpp"

namespace 
{
	float rectX, rectY;
	float speedX;
}

namespace n_game
{
	bool Game_Start()
	{
		// �Q�[���J�n���̏�����
		rectX = 100.0f;
		rectY = 100.0f;
		speedX = 200.0f; // 1�b������200�s�N�Z���ړ�
		return true;
	}

	void Game_Update(float deltaTime)
	{
		// �ȈՓ���: ���� �L�[�Ŏl�p�����E�ɓ�����
		if (n_input::IsKeyDown(VK_LEFT))  rectX -= speedX * deltaTime;
		if (n_input::IsKeyDown(VK_RIGHT)) rectX += speedX * deltaTime;

		// ��ʒ[�Œ��˕Ԃ�
		const float winW = 1920.0f - 50.0f; // �l�p�̕� 50px
		if (rectX < 0) { rectX = 0; speedX = -speedX; }
		if (rectX > winW) { rectX = winW; speedX = -speedX; }
	}

	// �V�[����̊e�I�u�W�F�N�g�ɑ΂��āu�ǂ��`�悷�邩�v���w������
	void Game_Render()
	{
	
	}

	void Game_Shutdown()
	{
		// �Q�[���I�����̃N���[���A�b�v
		// ���ɉ������Ȃ����A�K�v�Ȃ炱���Ń��\�[�X����Ȃǂ��s��
	}
}