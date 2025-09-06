/*テクスチャを読み込むために*/

#include "../../../include/assets/assets_manager/texture_manager.hpp"
#include "../../../include/render.hpp" // DirectX11のデバイス取得用
#include <stb_image.h>
#include <stdexcept>
#include <d3d11.h>
#include <iostream>
#include <filesystem>
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION

using namespace n_texturemanager;

void texture_manager::LoadAllTextures()
{

	namespace fs = std::filesystem;


    std::cout
        << "[TextureManager] BaseDir    = " << m_baseDir << "\n"
        << "[TextureManager] exists     = " << std::boolalpha
        << fs::exists(m_baseDir) << "\n"
        << "[TextureManager] is_directory = "
        << fs::is_directory(m_baseDir) << "\n";


	if (!fs::exists(m_baseDir) || !fs::is_directory(m_baseDir)) {
		std::cerr << "Texture directory not found: " << m_baseDir << "\n";
		return;
	}

	size_t count = 0;
	for (auto& entry : fs::directory_iterator(m_baseDir)) {
		if (!entry.is_regular_file()) continue;
		auto ext = entry.path().extension().string();
		if (ext != ".png" && ext != ".jpg" && ext != ".jpeg") continue;
		LoadTextureFromFile(entry.path());
		++count;
	}

	std::cout << "[TextureManager] Loaded textures: "
		<< count << " from " << m_baseDir << "\n";
}


/// 画像の読み込みは出来ているが、ストックが出来ていない　そのためにAssetsウィンドウに画像が表示されない

void texture_manager::LoadTextureFromFile(const std::filesystem::path& filepath)
{
    std::cout << "[TextureManager] Loading texture from: "
        << filepath << std::endl;

    // 1) ファイル読み込み (RGBA)
    int w, h, channels;
    unsigned char* data = stbi_load(
        filepath.string().c_str(), &w, &h, &channels, 4);
    if (!data) {
        std::string reason = stbi_failure_reason();
        throw std::runtime_error(
            "stbi_load failed: " + filepath.string() + " (" + reason + ")");
    }

    // 2) D3D11 テクスチャ作成
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = w * 4;

    ID3D11Texture2D* tex2d = nullptr;
    HRESULT hr = n_render::Render_GetDevice()->CreateTexture2D(&desc, &initData, &tex2d);
    stbi_image_free(data);
    if (FAILED(hr) || !tex2d) {
        throw std::runtime_error("CreateTexture2D failed: 0x"
            + std::to_string(hr));
    }

    // 3) SRV (ShaderResourceView) を作成
    ID3D11ShaderResourceView* srv = nullptr;
    hr = n_render::Render_GetDevice()->CreateShaderResourceView(tex2d, nullptr, &srv);
    tex2d->Release();  // テクスチャ本体は不要なので Release
    if (FAILED(hr) || !srv) {
        throw std::runtime_error("CreateSRV failed: 0x"
            + std::to_string(hr));
    }

    // 4) Texture 構造体を作成・初期化
    Texture tex;
    tex.name = filepath.filename().string();
    tex.width = w;
    tex.height = h;
    tex.tx_id = (ImTextureID)srv;  // ImGui は void* 扱い

    // 5) マップに必ず登録する
    m_Textures[tex.name] = tex;
    std::cout << "[TextureManager] Inserted: "
        << tex.name << " (Total=" << m_Textures.size() << ")\n";
}


Texture* texture_manager::GetTextureName(const std::string& name)
{
	auto it = m_Textures.find(name);
	if (it != m_Textures.end())
	{
		return &it->second;
	}
	return nullptr;
}


const std::unordered_map<std::string, Texture>& texture_manager::GetTextureNames() const
{
	return m_Textures;
}