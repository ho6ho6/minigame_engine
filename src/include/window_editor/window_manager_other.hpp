/*�T�u�E�B���h�E�p window_i*/
/*Scene,Game,Input���K����������K�v������C���^�t�F�[�X*/

#ifndef WINDOW_MANGAER_OTHER
#define WINDOW_MANGAER_OTHER

#pragma once

namespace engine::window {

    class window_manager_other {
    public:
        virtual      ~window_manager_other() = default;
        virtual void window_manager_other_Render() = 0;          // ���t���[���Ăяo�����`��֐�
        virtual bool window_manager_other_IsVisible() const = 0;  // �\���^��\���Ǘ�
    };

} // namespace engine::window

#endif // !WINDOW_MANGAER_OTHER