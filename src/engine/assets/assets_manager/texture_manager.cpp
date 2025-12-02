/*テクスチャを読み込むために*/

#include "include/assets/assets_manager/texture_manager.hpp"
#include "include/render.hpp" // DirectX11のデバイス取得用
#include <stb_image.h>
#include <stdexcept>
#include <d3d11.h>
#include <iostream>
#include <filesystem>
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION

namespace n_texturemanager 
{

    // exeファイルの場所から候補を探して返す
    static std::filesystem::path ResolveTexturesDir()
    {
        namespace fs = std::filesystem;

        // exeファイルの場所を取得
        char exePath[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
        if (len == 0 || len == MAX_PATH)
        {
            std::cerr << "[TextureManager] GetModuleFileNameA failed\n";
            return {};
        }
        fs::path exeDir = fs::path(exePath).remove_filename();

        // 候補リスト
        std::vector<fs::path> candidates = {
            exeDir / "Assets" / "textures", // exe直下
            exeDir.parent_path() / "Assets" / "textures", // 1階層上
            fs::current_path() / "Assets" / "textures" // カレントディレクトリ
        };

        for (auto& c : candidates)
        {
            if (fs::exists(c) && fs::is_directory(c))   return c;
        }

        // 見つからなかった
        std::cerr << "[TextureManager] Texture directory not found in candidates\n";
        for (auto& c : candidates)
        {
            std::cerr << "  Candidate: " << c << "\n";
        }
        return {};
    }


	texture_manager instance_texmag(ResolveTexturesDir());

	// テクスチャ名で取得
    std::vector<std::string> texture_manager::GetTextureKeys() const {
        std::vector<std::string> keys;
        keys.reserve(m_Textures.size());
        for (const auto& kv : m_Textures) keys.push_back(kv.first);
        return keys;
    }

    texture_manager::~texture_manager()
    {
        // 登録されたすべてのテクスチャの SRV を解放
        for (auto& kv : m_Textures) {
            ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(kv.second.tx_id);
            if (srv) {
                srv->Release();
            }
        }
        m_Textures.clear();
	}

    void texture_manager::LoadAllTextures() // 起動時に一度だけ
    {

        //printf("[texture_manager/LoadAllTextures/DBG] instance_texmag addr=%p file=%s\n", (void*)&instance_texmag, __FILE__);

        namespace fs = std::filesystem;

        if (m_baseDir.empty()) {
            std::cerr << "[TextureManager] base dir is empty, skipping LoadAllTextures\n";
            return;
        }

        // base 絶対パス化 -- 相対パス／カレントディレクトリ問題の検出用
        fs::path base = fs::absolute(m_baseDir);
        std::cout << "[TextureManager] BaseDir= " << "\n" << base 
                  << " cwd=" << fs::current_path() << "\n";

        if (!fs::exists(base) || !fs::is_directory(base))
        {
            std::cerr << "[TextureManager] Texture directory not found: " << "\n" << base
                      << "cwd = " << fs::current_path() << "\n";
            return;
        }

        //std::cout
        //    << "[TextureManager] BaseDir    = " << base << "\n"
        //    << "[TextureManager] exists     = " << std::boolalpha
        //    << fs::exists(base) << "\n"
        //    << "[TextureManager] is_directory = "
        //    << fs::is_directory(base) << "\n";


        //if (!fs::exists(base) || !fs::is_directory(base)) {
        //    std::cerr << "[TextureManager] Texture directory not found: " << base << "\n";
        //    return;
        //}

        size_t count = 0;
        for (auto& entry : fs::directory_iterator(base)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension().string();
            if (ext != ".png" && ext != ".jpg" && ext != ".jpeg") continue;
            LoadTextureFromFile(entry.path());
            ++count;
        }

        std::cout << "[TextureManager] Loaded textures: "
            << count << " from " << base << "\n";
    }


    void texture_manager::LoadTextureFromFile(const std::filesystem::path& filepath) // テクスチャを1つ読み込む
    {
        std::cout << "[TextureManager] Loading texture from: "
            << filepath << std::endl;

        // ファイル読み込み (RGBA)
        int w, h, channels;
        unsigned char* data = stbi_load(
            filepath.string().c_str(), &w, &h, &channels, 4);
        if (!data) {
            std::string reason = stbi_failure_reason();
            throw std::runtime_error(
                "[TextureManager] stbi_load failed: " + filepath.string() + " (" + reason + ")");
        }

        // D3D11 テクスチャ作成
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
            throw std::runtime_error("[TextureManager] CreateTexture2D failed: 0x"
                + std::to_string(hr));
        }

        // SRV (ShaderResourceView) を作成
        ID3D11ShaderResourceView* srv = nullptr;
        hr = n_render::Render_GetDevice()->CreateShaderResourceView(tex2d, nullptr, &srv);
        tex2d->Release();  // テクスチャ本体は不要なので Release
        if (FAILED(hr) || !srv) {
            throw std::runtime_error("[TextureManager] CreateSRV failed: 0x"
                + std::to_string(hr));
        }

        // Texture 構造体を作成・初期化
        Texture tex;
        tex.name = filepath.filename().string();
		
        /*実際に使用されるアセットオブジェクトの大きさを決定*/
        tex.width = 64;     // window_scene.cpp に置かれるオブジェクトのサイズx
		tex.height = 64;    // window_scene.cpp に置かれるオブジェクトのサイズy

        tex.tx_id = (ImTextureID)srv;  // ImGui は void* 扱い

        // Debug
        printf("[TextureManager/DBG] Loaded Texture name=%s width=%d height=%d; srv=%p\n",
            tex.name.c_str(), tex.width, tex.height, (void*)tex.tx_id);


        // マップに登録
        m_Textures[tex.name] = tex;
        std::cout << "[TextureManager] Inserted: "
            << tex.name << " (Total=" << m_Textures.size() << ")\n";
    }

    const std::unordered_map<std::string, Texture>& texture_manager::GetTextureNames() const
    {
        return m_Textures;
    }

}