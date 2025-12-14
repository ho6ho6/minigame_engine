#include <iostream>
#include <imgui.h>
#include "include/assets/assets_manager/texture_manager.hpp"
#include "include/assets/texture.hpp"
#include "include/assets/assets_manager/assets_manager.hpp"


void OnAssetClicked(const std::string& name);


void n_assetsmanager::assets_manager::assets_Show()
{
	ImGui::Begin("Assets");

	//  インスタンス m_TextureManager から呼び出す
	const auto& textures = m_TextureManager.GetTextureNames();


	//ImGui::Text("LoadedCount = %zu", textures.size());   // 読み込んだテクスチャ数
	//ImGui::Separator();                             // 区切り線

	// テクスチャ数の出力デバック用
	//std::cout << "[AssetsWindow] Texture count = "
	//    << textures.size() << std::endl;


	const ImVec2 thumbSize(64, 64); // サムネイルの表示サイズ
	const float padding = 32.0f;     // サムネイル間の余白
	const float cellSize = thumbSize.x + padding; // セルのサイズ（サムネイル + 余白）
	float panelWidth = ImGui::GetContentRegionAvail().x; // 利用可能なパネル幅
	int columns = std::max(1, (int)(panelWidth / cellSize)); // 列数を計算
	ImGui::Columns(columns, nullptr, false); // 列を設定

	int index = 0;
	for (auto& kv : textures)
	{
		const std::string& name = kv.first;
		const Texture& tex = kv.second;

		ImGui::PushID(index); // ユニークIDを設定

		// 画像ボタン ImageButtonを用いる
		if (tex.tx_id)
		{
			if (ImGui::ImageButton(name.c_str(), (ImTextureID)tex.tx_id, thumbSize))
			{	// クリックされたときの処理
				printf("[AssetsWindow] Clicked on texture \n");
				OnAssetClicked(name);
			}

			// ドラッグ＆ドロップのソース設定
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				//printf("[Assetes_manager] テクスチャ選択 PAYLOAD");

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					//printf("[AssetsWindow] Begin DragDrop Source \n");

					const char* d = name.c_str();
					//printf("Send payload name='%s' size=%d \n", d, (int)name.size() + 1);
					/*for (int i = 0; i < (int)name.size() + 1 && i < 32; ++i) {
						printf(" %02X \n", (unsigned char)d[i]);
					}*/


					// ペイロードにアセット名を設定
					ImGui::SetDragDropPayload("ASSET_PAYLOAD", (const void*)name.c_str(), (int)name.size() + 1); // +1はnull終端のため
					ImGui::Image((ImTextureID)tex.tx_id, thumbSize); // ドラッグ中に表示するプレビュー
					ImGui::TextUnformatted(name.c_str());
					ImGui::EndDragDropSource();
					// window_scene.cppで受け取る処理がある
				}
			}
		}
		else
		{
			ImGui::Button("No Tex", thumbSize); // テクスチャが無い場合の代替表示
		}

		// テクスチャ名を表示
		ImGui::TextWrapped("%s", name.c_str());

		ImGui::PopID(); // IDを戻す

		ImGui::NextColumn(); // 次の列へ
		++index;
	}	// for ループの終わり こっちが本当の終わり

	ImGui::Columns(1); // 列設定を解除

	ImGui::End();
}


// アセットがクリックされたときの処理
void OnAssetClicked(const std::string& name)
{
	// クリックされたアセット名をコンソールに出力
	printf("[AssetsWindow] Asset clicked \n");
}