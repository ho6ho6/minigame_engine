/************************************************************
 *�L�[�{�[�h�E�}�E�X��Ԃ̎��W�E�₢���킹�C���^�t�F�[�X�錾*
 ************************************************************/

#pragma once
#include <array>
#include <windows.h>

/*���͂��󂯎��*/

namespace Input
{
    // �������^�I��
    void InitInput();
    void ShutdownInput();

    // �t���[�����ƂɌĂяo���ď�ԍX�V
    void ProcessInput();

    // ��ԃN�G��
    bool IsKeyDown(int vk);      // �������ςȂ�
    bool IsKeyPressed(int vk);   // �������u��
    bool IsKeyReleased(int vk);  // �������u��
}