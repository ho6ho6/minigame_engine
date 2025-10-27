#include <iostream>
#include <imgui.h>
#include "include/assets/assets_manager/texture_manager.hpp"
#include "include/assets/texture.hpp"
#include "include/assets/assets_manager/assets_manager.hpp"


void OnAssetClicked(const std::string& name);


void n_assetsmanager::assets_manager::assets_Show()
{
    ImGui::Begin("Assets");

    //  �C���X�^���X m_TextureManager ����Ăяo��
    const auto& textures = m_TextureManager.GetTextureNames();

	
    //ImGui::Text("LoadedCount = %zu", textures.size());   // �ǂݍ��񂾃e�N�X�`����
	//ImGui::Separator();                             // ��؂��

    // �e�N�X�`�����̏o�̓f�o�b�N�p
    //std::cout << "[AssetsWindow] Texture count = "
    //    << textures.size() << std::endl;


	const ImVec2 thumbSize(64, 64); // �T���l�C���̕\���T�C�Y
	const float padding = 32.0f;     // �T���l�C���Ԃ̗]��
	const float cellSize = thumbSize.x + padding; // �Z���̃T�C�Y�i�T���l�C�� + �]���j
	float panelWidth = ImGui::GetContentRegionAvail().x; // ���p�\�ȃp�l����
	int columns = std::max(1, (int)(panelWidth / cellSize)); // �񐔂��v�Z
	ImGui::Columns(columns, nullptr, false); // ���ݒ�

	int index = 0;
	for (auto& kv : textures)
	{
		const std::string& name = kv.first;
		const Texture& tex = kv.second;

		ImGui::PushID(index); // ���j�[�NID��ݒ�

		// �摜�{�^�� ImageButton��p����
		if (tex.tx_id)
		{
			if (ImGui::ImageButton(name.c_str(), (ImTextureID)tex.tx_id, thumbSize))
			{	// �N���b�N���ꂽ�Ƃ��̏���
				printf("[AssetsWindow] Clicked on texture \n");
				OnAssetClicked(name);
			}
			
			// �h���b�O���h���b�v�̃\�[�X�ݒ�
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				//printf("[Assetes_manager] �e�N�X�`���I�� PAYLOAD");

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) 
				{	
					//printf("[AssetsWindow] Begin DragDrop Source \n");
					
					const char* d = name.c_str();
					printf("Send payload name='%s' size=%d \n", d, (int)name.size() + 1);
					/*for (int i = 0; i < (int)name.size() + 1 && i < 32; ++i) {
						printf(" %02X \n", (unsigned char)d[i]);
					}*/


					// �y�C���[�h�ɃA�Z�b�g����ݒ�
					ImGui::SetDragDropPayload("ASSET_PAYLOAD", (const void*)name.c_str(), (int)name.size() + 1); // +1��null�I�[�̂���
					ImGui::Image((ImTextureID)tex.tx_id, thumbSize); // �h���b�O���ɕ\������v���r���[
					ImGui::TextUnformatted(name.c_str());
					ImGui::EndDragDropSource();
					// window_scene.cpp�Ŏ󂯎�鏈��������
				}
			}
		}
		else
		{
			ImGui::Button("No Tex", thumbSize); // �e�N�X�`���������ꍇ�̑�֕\��
		}

		// �e�N�X�`������\��
		ImGui::TextWrapped("%s", name.c_str());

		ImGui::PopID(); // ID��߂�

		ImGui::NextColumn(); // ���̗��
		++index;
	}	// for ���[�v�̏I��� ���������{���̏I���

	ImGui::Columns(1); // ��ݒ������

    ImGui::End();
}


// �A�Z�b�g���N���b�N���ꂽ�Ƃ��̏���
void OnAssetClicked(const std::string& name)
{
	// �N���b�N���ꂽ�A�Z�b�g�����R���\�[���ɏo��
	printf("[AssetsWindow] Asset clicked \n");
}
