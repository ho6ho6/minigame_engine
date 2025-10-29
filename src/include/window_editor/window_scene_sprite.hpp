/*scene�Ƀe�N�X�`���𗎂Ƃ����ނ��߂ɍ\����sprite��錾*/

#ifndef SCENE_SPRITE
#define SCENE_SPRITE

#pragma once
#include <string>
#include "include/assets/texture.hpp"


// scene�ɕ\�����邽�߂̍\����
struct SceneSprite
{
    std::string name;     // �X�v���C�g���i�e�N�X�`�����Ɠ����ŗǂ��j
    Texture* texture;    // �e�N�X�`���n���h��
    float pos_x = 0.0f;   // �V�[������X���W
    float pos_y = 0.0f;   // �V�[������Y���W
    int width = 32;  // �X�v���C�g�̕�
    int height = 32; // �X�v���C�g�̍���
    int z_order = 1;   // �`�揇���i�傫���قǑO�ʂɕ`��j
    bool selected = false; // �I�����
};

#endif // !SCENE_SPRITE
