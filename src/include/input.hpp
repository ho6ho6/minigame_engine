/************************************************************
 *�L�[�{�[�h�E�}�E�X��Ԃ̎��W�E�₢���킹�C���^�t�F�[�X�錾*
 ************************************************************/

#ifndef INPUT_HPP
#define INPUT_HPP

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
    bool IsKeyDown(int key);      // �������ςȂ�
    bool IsKeyPressed(int key);   // �������u��
    bool IsKeyReleased(int key);  // �������u��
}

#endif // INPUT_HPP