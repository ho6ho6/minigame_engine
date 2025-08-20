/**************************************************************
*�V�[���f�[�^���󂯎��A�`��R�}���h�𔭍s����C���^�t�F�[�X			    *
**************************************************************/

#ifndef RENDER_HPP
#define RENDER_HPP


#pragma once
#include <windows.h>
#include <cstdint>
#include <d3d11.h>

namespace render
{
	bool Render_Start(HWND hwnd, int width, int height);    // GDI+ ������
	void Render_Shutdown();                                    // GDI+ �I������

    // �����ɒǉ��F���t���[���Ăяo�� Render �֐�
    void Render_Update(float , uint64_t );

	//   clearColor: {r, g, b, a}
	void Render_BegineFrame(const float clearColor[4]);

	// �`��R�}���h�m��iPresent �͍s��Ȃ��j
	void Render_EndFrame();

	// SwapChain��Present
	void Render_Present();

	// �r���[�|�[�g�̃T�C�Y�ύX
	void Render_Resizeviewport(int width, int height);

	// ��� (x, y) ����I�u�W�F�N�g���s�b�N���AID ��Ԃ�
    // -1 ��Ԃ�����u�����I������Ă��Ȃ��v
	int Render_PickObject(int x, int y);

	ID3D11ShaderResourceView* Render_GetSceneSRV(); // �r���[�|�[�g�̃e�N�X�`�����擾
	// DirectX 11 �̃f�o�C�X�ƃR���e�L�X�g���擾
	ID3D11Device*				Render_GetDevice();
	ID3D11DeviceContext*		Render_GetDeviceContext();

}

#endif // RENDER_HPP