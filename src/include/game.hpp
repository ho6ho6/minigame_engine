/************************************************************
 *�Q�[���S�̂̏�ԊǗ��E�V�[�������E�X�e�[�g�}�V���Ȃǂ�錾*
 ************************************************************/

#ifndef GAME_HPP
#define GAME_HPP


#pragma once

namespace game
{
	//�Q�[���S�̂̏������B���\�[�X�ǂݍ��݂�ϐ�������
	bool Game_Start();

	//���t���[���Ă΂��X�V�֐��B�����@deltaTime
	void Game_Update(float deltaTime);

	//���t���[���Ă΂��`��֐�
	void Game_Render();

	//�Q�[���I�����ɌĂ΂��B���\�[�X�����ϐ��̃N���[���A�b�v
	void Game_Shutdown();
}

#endif // GAME_HPP