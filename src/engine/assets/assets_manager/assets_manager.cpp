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

		/*ImGui::PushID(index); // ユニークIDを設定

		// 画像ボタン ImageButtonを用いる
		if (tex.tx_id)
		{
			//if (ImGui::ImageButton(name.c_str(), (ImTextureID)tex.tx_id, thumbSize))
			//{	// クリックされたときの処理
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
			ImGui::Button("No Tex", thumbSize); // テクスチャが無い場合の代替表示
		}

		// テクスチャ名を表示
		ImGui::TextWrapped("%s", name.c_str());

		// コンテキストメニュー
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

		ImGui::PopID(); // IDを戻す

		ImGui::NextColumn(); // 次の列へ
		++index;
	}	// for ループの終わり こっちが本当の終わり

	ImGui::Columns(1); // 列設定を解除
	*/
	} // for ループの終わり 便宜上ここに}を置いてる
    ImGui::End();
}


/*
// アセットがクリックされたときの処理
void OnAssetClicked(const std::string& name)
{
	// クリックされたアセット名をコンソールに出力
	printf("[AssetsWindow] Asset clicked \n");
}


void OnAssetDoubleClicked(const std::string& name)
{
	// ダブルクリックされたアセット名をコンソールに出力
	printf("[AssetsWindow] Asset double-clicked \n");

	// sceneにDD
	ImVec2 mousePos = ImGui::GetMousePos();

	// シーンにアセットを配置する
	//n_windowscene::window_scene::Get().AddAssetToScene(name, mousePos.x, mousePos.y);
}

void OnShowExplorer(const std::string& name)
{
	// アセット名をコンソールに出力
	printf("[AssetsWindow] Show in Explorer \n");
	// ファイルエクスプローラーでアセットの場所を開く
	std::string command = "explorer.exe /select,\"" + std::string("../../Assets/textures/") + name + "\"";
	system(command.c_str());
}
*/