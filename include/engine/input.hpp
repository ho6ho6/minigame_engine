/************************************************************
 *�L�[�{�[�h�E�}�E�X��Ԃ̎��W�E�₢���킹�C���^�t�F�[�X�錾*
 ************************************************************/

#pragma once
#include <array>
#include <windows.h>

/*���͂��󂯎��*/

namespace input
{
    // �������^�I��
    void Input_Start();
    void Input_Shutdown();

    // �t���[�����ƂɌĂяo���ď�ԍX�V
    void Input_Update();

    // ��ԃN�G��
    bool IsKeyDown(int vk);      // �������ςȂ�
    bool IsKeyPressed(int vk);   // �������u��
    bool IsKeyReleased(int vk);  // �������u��
}