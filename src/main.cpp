#include "input.hpp"
#include "renndert.hpp"

/*�Q�[���G���W���̒��S*/

int main()
{
    InitWindow();       // �E�B���h�E�������iWin32 API�Ȃǁj
    InitInput();        // ���͏�����
    InitRenderer();     // �`�揉����

    while (IsRunning()) {
        ProcessInput(); // ���͏���
        Update();       // �Q�[����ԍX�V�i��Œǉ��j
        Render();       // �`�揈��
    }

    Shutdown();         // �I������
    return 0;
}