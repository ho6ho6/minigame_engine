#include <iostream>
#include <imgui.h>
#include "../../../include/assets/assets_manager/texture_manager.hpp"
#include "../../../include/assets/texture.hpp"
#include "../../../include/assets/assets_manager/assets_manager.hpp"

void n_assetsmanager::assets_manager::assets_Show()
{
    ImGui::Begin("Assets");

    //  インスタンス m_TextureManager から呼び出す
    const auto& map = m_TextureManager.GetTextureNames();
    ImGui::Text("LoadedCount = %zu", map.size());
    ImGui::Separator();

    // テクスチャ数の出力デバック用
    //std::cout << "[AssetsWindow] Texture count = "
    //    << map.size() << std::endl;

    // 実際に Image ボタンを並べる
    for (auto& [name, tex] : map) {
        ImGui::Text("%s", name.c_str());
        if (tex.tx_id) {
            ImGui::Image(tex.tx_id, ImVec2(64, 64));
        }
    }

    ImGui::End();
}
