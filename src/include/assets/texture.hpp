/*テクスチャのひな形*/

#ifndef TEXTURE
#define TEXTURE

#pragma once
#include <string>
#include <imgui.h>

struct Texture
{
    /// 画像ファイル名（例："grass.png"）
    std::string name;

    /// ImGui に渡すテクスチャハンドル（DirectX11 なら ID3D11ShaderResourceView* をキャスト）
    ImTextureID tx_id;

    /// 元画像の幅・高さ（必要に応じて参照用）
    int width = 0;
    int height = 0;
};

#endif // !TEXTURE
