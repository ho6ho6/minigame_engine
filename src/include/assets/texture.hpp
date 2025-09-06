/*�e�N�X�`���̂ЂȌ`*/

#ifndef TEXTURE
#define TEXTURE

#pragma once
#include <string>
#include <imgui.h>

struct Texture
{
    /// �摜�t�@�C�����i��F"grass.png"�j
    std::string name;

    /// ImGui �ɓn���e�N�X�`���n���h���iDirectX11 �Ȃ� ID3D11ShaderResourceView* ���L���X�g�j
    ImTextureID tx_id;

    /// ���摜�̕��E�����i�K�v�ɉ����ĎQ�Ɨp�j
    int width = 0;
    int height = 0;
};

#endif // !TEXTURE
