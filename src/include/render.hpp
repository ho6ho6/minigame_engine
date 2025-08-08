/**************************************************************
*�V�[���f�[�^���󂯎��A�`��R�}���h�𔭍s����C���^�t�F�[�X			    *
**************************************************************/

#ifndef RENDER_HPP
#define RENDER_HPP


#pragma once
#include <windows.h>
#include <cstdint>

namespace render
{
	bool Render_Start(HWND hwnd, int width, int height);    // GDI+ ������
	void Render_Shutdown();                                    // GDI+ �I������

    // �����ɒǉ��F���t���[���Ăяo�� Render �֐�
    void Render_Update(float , uint64_t );
}

#endif // RENDER_HPP