#pragma once
#include <array>
#inculde <windows.h>

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