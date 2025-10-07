#include <iostream>
#include <imgui.h>
#include "../../../include/assets/assets_manager/texture_manager.hpp"
#include "../../../include/assets/texture.hpp"
#include "../../../include/assets/assets_manager/assets_manager.hpp"

/*
void OnAssetClicked(const std::string& name);
void OnAssetDoubleClicked(const std::string& name);
void OnShowExplorer(const std::string& name);
*/

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

		/*ImGui::PushID(index); // ���j�[�NID��ݒ�

		// �摜�{�^�� ImageButton��p����
		if (tex.tx_id)
		{
			//if (ImGui::ImageButton(name.c_str(), (ImTextureID)tex.tx_id, thumbSize))
			//{	// �N���b�N���ꂽ�Ƃ��̏���
			//	printf("[AssetsWindow] Clicked on texture \n");
			//	OnAssetClicked(name);

			//}
			if (ImGui::Button("test_Button", ImVec2(64, 64)))
			{
				printf("[AssetsWindow] Clicked on texture \n");
			}
		}
		else
		{
			ImGui::Button("No Tex", thumbSize); // �e�N�X�`���������ꍇ�̑�֕\��
		}

		// �e�N�X�`������\��
		ImGui::TextWrapped("%s", name.c_str());

		// �R���e�L�X�g���j���[
		if (ImGui::BeginPopup("asset_ctx"))
		{
			if (ImGui::MenuItem("place in Scene"))
			{
				OnAssetDoubleClicked(name);
			}
			if (ImGui::MenuItem("Show in Explorer"))
			{
				OnShowExplorer(name);
			}
			ImGui::EndPopup();
		}

		ImGui::PopID(); // ID��߂�

		ImGui::NextColumn(); // ���̗��
		++index;
	}	// for ���[�v�̏I��� ���������{���̏I���

	ImGui::Columns(1); // ��ݒ������
	*/
	} // for ���[�v�̏I��� �֋X�ケ����}��u���Ă�
    ImGui::End();
}


/*
// �A�Z�b�g���N���b�N���ꂽ�Ƃ��̏���
void OnAssetClicked(const std::string& name)
{
	// �N���b�N���ꂽ�A�Z�b�g�����R���\�[���ɏo��
	printf("[AssetsWindow] Asset clicked \n");
}


void OnAssetDoubleClicked(const std::string& name)
{
	// �_�u���N���b�N���ꂽ�A�Z�b�g�����R���\�[���ɏo��
	printf("[AssetsWindow] Asset double-clicked \n");

	// scene��DD
	ImVec2 mousePos = ImGui::GetMousePos();

	// �V�[���ɃA�Z�b�g��z�u����
	//n_windowscene::window_scene::Get().AddAssetToScene(name, mousePos.x, mousePos.y);
}

void OnShowExplorer(const std::string& name)
{
	// �A�Z�b�g�����R���\�[���ɏo��
	printf("[AssetsWindow] Show in Explorer \n");
	// �t�@�C���G�N�X�v���[���[�ŃA�Z�b�g�̏ꏊ���J��
	std::string command = "explorer.exe /select,\"" + std::string("../../Assets/textures/") + name + "\"";
	system(command.c_str());
}
*/