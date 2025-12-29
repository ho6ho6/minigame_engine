/*テクスチャを読み込むために*/

#include "include/assets/assets_manager/texture_manager.hpp"
#include "include/render.h" // DirectX11のデバイス取得用
#include <stb_image.h>
#include <d3d11.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <atomic>

namespace n_texturemanager {
    namespace fs = std::filesystem;

    /*内部型定義（cpp限定）*/
    struct ImageData {
        int w = 0;
        int h = 0;
        std::vector<unsigned char> pixels; // RGBA
    };

    // リクエストは unique_ptr でキューに入れる（LoadRequest はコピー禁止）
    static std::deque<std::unique_ptr<LoadRequest>> g_reqQueue;
    static std::mutex g_reqMutex;
    static std::condition_variable g_reqCv;

    // 結果キューは値で扱う（LoadResult はムーブ可能）
    static std::deque<LoadResult> g_resQueue;
    static std::mutex g_resMutex;

    static std::atomic<uint64_t> g_nextRequestId{ 1 };
    static std::atomic<bool> g_workerRunning{ false };
    static std::thread g_workerThread;

    static std::mutex g_texturesMutex; // m_Textures は主にメインスレッドで扱うが、GetTextureByName などが別スレッドから呼ばれる可能性を考慮

    // ロード完了通知コールバック（メインスレッドで呼ばれる）
    static texture_manager::LoadCallback g_loadCallback = nullptr;

    /*forward declarations(内部関数)*/
    static void WorkerThreadFunc();
    //static void EnqueueRequest(LoadRequest&& req);
    static void PushResult(LoadResult&& res);
    static std::deque<LoadResult> PopAllResults();

    void StartWorker();
    void StopWorker();

    /*instance_texmag(シングルトン)*/
    texture_manager& instance_texmag() {
        static texture_manager inst;
        return inst;
    }

    /*texture_manager 実装*/
    texture_manager::~texture_manager()
    {
        // Ensure clean shutdown if user forgot to call Shutdown()
        try {
            Shutdown();
        }
        catch (const std::exception& e)
        {
            // 標準エラー出力へログ
            std::cerr << "[TextureManager] Shutdown threw exception: " << e.what() << "\n";
        #ifdef _WIN32
            ::OutputDebugStringA("[TextureManager] Shutdown threw exception: ");
            ::OutputDebugStringA(e.what());
            ::OutputDebugStringA("\n");
        #endif
                // 最小限の後片付けを試みる（例外を投げないように個別に捕捉）
            try { StopWorker(); }   catch (...) {}
            try { UnloadAll(); }    catch (...) {}
        }
        catch (...)
        {
                // 非 std::exception 型の例外を捕捉
            std::cerr << "[TextureManager] Shutdown threw unknown exception\n";
        #ifdef _WIN32
            ::OutputDebugStringA("[TextureManager] Shutdown threw unknown exception\n");
        #endif
            try { StopWorker(); }   catch (...) {}
            try { UnloadAll(); }    catch (...) {}
        }
    }

    bool texture_manager::Initialize(const std::filesystem::path& baseDir)
    {
        if (baseDir.empty()) {
            std::cerr << "[TextureManager] Initialize: baseDir is empty\n";
            return false;
        }
        m_baseDir = fs::absolute(baseDir);
        if (!fs::exists(m_baseDir) || !fs::is_directory(m_baseDir)) {
            std::cerr << "[TextureManager] Initialize: baseDir invalid: " << m_baseDir << "\n";
            return false;
        }

        // ワーカー起動
        if (!g_workerRunning.load()) {
            StartWorker();
        }

        return true;
    }

    void texture_manager::Shutdown()
    {
        // stop worker
        if (g_workerRunning.load()) {
            StopWorker();
        }

        // メインスレッドで残りの結果を処理してGPUリソ​​ースを作成するか、削除
        ProcessPendingResults();

        // Release GPU resources
        UnloadAll();
    }

    size_t texture_manager::LoadAllTextures()
    {
        if (m_baseDir.empty()) {
            std::cerr << "[TextureManager] LoadAllTextures: base dir is empty\n";
            return 0;
        }

        fs::path base = fs::absolute(m_baseDir);
        if (!fs::exists(base) || !fs::is_directory(base)) {
            std::cerr << "[TextureManager] LoadAllTextures: directory not found: " << base << "\n";
            return 0;
        }

        size_t count = 0;
        for (auto& entry : fs::directory_iterator(base)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension().string();
            // 小文字/大文字の差を吸収
            for (auto& c : ext) c = (char)tolower(c);
            if (ext != ".png" && ext != ".jpg" && ext != ".jpeg") continue;
            if (LoadTextureFromFile(entry.path())) ++count;
        }

        std::cout << "[TextureManager] Loaded textures: " << count << " from " << base << "\n";
        return count;
    }

    bool texture_manager::LoadTextureFromFile(const std::filesystem::path& filepath)
    {
        // 同期ロード：ファイル読み込み→GPU作成→登録
        int w = 0, h = 0, channels = 0;
        unsigned char* data = stbi_load(filepath.string().c_str(), &w, &h, &channels, 4);
        if (!data) {
            std::cerr << "[TextureManager] stbi_load failed: " << filepath << " reason=" << stbi_failure_reason() << "\n";
            return false;
        }

        Texture tex;
        tex.name = filepath.filename().string();
        tex.width = w;
        tex.height = h;

        //printf("[TextureManager]Loaded image %s: w=%d h=%d tex.width=%d tex.height=%d\n",
        //    filepath.string().c_str(), w, h, tex.width, tex.height);

        bool ok = CreateGpuTextureFromMemory(data, w, h, tex);
        stbi_image_free(data);
        if (!ok) {
            std::cerr << "[TextureManager] CreateGpuTextureFromMemory failed for: " << filepath << "\n";
            return false;
        }

        {
            std::lock_guard<std::mutex> lk(g_texturesMutex);
            // 既存があれば Release して上書き
            auto it = m_Textures.find(tex.name);
            if (it != m_Textures.end()) {
                ID3D11ShaderResourceView* old = reinterpret_cast<ID3D11ShaderResourceView*>(it->second.tx_id);
                if (old) old->Release();
            }
            m_Textures[tex.name] = std::move(tex);
        }

        return true;
    }

    // 非同期ロードAPI
    RequestId texture_manager::LoadTextureAsync(const std::filesystem::path& filepath)
    {
        RequestId id = g_nextRequestId.fetch_add(1);
        LoadRequest req;
        req.id = id;
        req.path = filepath;
        EnqueueRequest(std::move(req));
        return id;
    }

    // 要求を追加する関数
    void texture_manager::EnqueueRequest(LoadRequest&& req)
    {
        auto p = std::make_unique<LoadRequest>(std::move(req));
        {
            std::lock_guard<std::mutex> lk(g_reqMutex);
            g_reqQueue.push_back(std::move(p));
        }
        g_reqCv.notify_one();
    }


    bool texture_manager::CancelLoad(uint32_t id)
    {
        // 既にキューにあるリクエストを探して cancel フラグを立てる
        std::lock_guard<std::mutex> lk(g_reqMutex);
        for (auto& up : g_reqQueue) {
            if (up && up->id == id) {
                up->cancel.store(true);
                return true;
            }
        }
        // 既にワーカーが取り出している場合はワーカー内でチェックするしかない
        return false;
    }

    // メインスレッドで呼ぶ: 結果キューを取り出して GPU 作成と登録を行う
    void texture_manager::ProcessPendingResults()
    {
        // まず全結果を取り出す（ロックは PopAllResults 内で行われる）
        std::deque<LoadResult> local = PopAllResults();

        int uploadsThisFrame = 0;
        const int maxUploadsPerFrame = 5; // フレームあたりの最大アップロード数を制限

        // デバイスはループ外で一度だけ取得
        ID3D11Device* device = n_render::Render_GetDevice();

		// コンテキストも
		ID3D11DeviceContext* context = n_render::Render_GetDeviceContext();

		// デバイスが取得できない場合はすべて結果を再キューして終了
        if (!device || !context)
        {
            for (auto& c : local)
            {
                PushResult(std::move(c));
            }
            return;
        }

        for (auto it = local.begin(); it != local.end(); ++it) {
            // 上限に達したら残りを再キューしてループを抜ける
            if (uploadsThisFrame >= maxUploadsPerFrame) {
                for (auto jt = it; jt != local.end(); ++jt) {
                    PushResult(std::move(*jt));
                }
                break;
            }

            LoadResult& r = *it;

            if (!r.success) {
                printf("Load failed id=%u err=%s\n", r.id, r.error.c_str());
                // UI に失敗を通知する（必要なら）
                if (g_loadCallback) {
                    LoadStatus st;
                    st.id = r.id;
                    st.state = LoadState::Failed;
                    st.errorMessage = r.error;
                    g_loadCallback(st);
                }
                // 失敗した結果は破棄して次へ
                continue;
            }

            // GPU リソース作成（メインスレッドで実行）
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv =
                CreateSRVFromMemoryStagingCopy(device, context, r.pixels.data(), r.width, r.height);

            // ピクセルデータはもう不要なので早めに解放してメモリを返す
            r.pixels.clear();
            r.pixels.shrink_to_fit();

            if (!srv) {
                printf("[ProcessPendingResults] CreateSRVFromMemoryStagingCopy failed for '%s'\n", r.name.c_str());
                if (g_loadCallback) {
                    LoadStatus st;
                    st.id = r.id;
                    st.state = LoadState::Failed;
                    st.errorMessage = "CreateSRVFromMemoryStagingCopy failed";
                    g_loadCallback(st);
                }
                continue;
            }

            // 登録（srv の所有は RegisterTextureFromSRV が保持する想定）
            try {
                RegisterTextureFromSRV(r.name, srv, r.width, r.height);
            }
            catch (const std::exception& e) {
                // 万が一登録で例外が出ても握りつぶしてログ
                std::cerr << "[ProcessPendingResults] RegisterTextureFromSRV threw: " << e.what() << "\n";
                if (g_loadCallback) {
                    LoadStatus st;
                    st.id = r.id;
                    st.state = LoadState::Failed;
                    st.errorMessage = e.what();
                    g_loadCallback(st);
                }
                continue;
            }
			catch (...) {   // 非 std::exception 型の例外捕捉
                std::cerr << "[ProcessPendingResults] RegisterTextureFromSRV threw unknown exception\n";
                if (g_loadCallback) {
                    LoadStatus st;
                    st.id = r.id;
                    st.state = LoadState::Failed;
                    st.errorMessage = "unknown exception";
                    g_loadCallback(st);
                }
                continue;
            }

            // 成功通知（UI 更新用）
            if (g_loadCallback) {
                LoadStatus st;
                st.id = r.id;
                st.state = LoadState::Completed;
                st.name = r.name;
                st.progress = 1.0f;
                g_loadCallback(st);
            }

            ++uploadsThisFrame;
        }
    }

    LoadStatus texture_manager::GetLoadStatus(RequestId id) const
    {
        // 簡易実装: 非同期キューの状態を確認する（詳細な状態管理は拡張可能）
        LoadStatus st;
        st.id = id;
        // check request queue
        {
            std::lock_guard<std::mutex> lk(g_reqMutex);
            for (const auto& r : g_reqQueue) {
                if (r->id == id) {
                    st.state = LoadState::Queued;
                    return st;
                }
            }
        }
        // check result queue (まだ最終化されていない結果)
        {
            std::lock_guard<std::mutex> lk(g_resMutex);
            for (const auto& res : g_resQueue) {
                if (res.id == id) {
                    st.state = res.success ? LoadState::ReadyToFinalize : LoadState::Failed;
                    st.errorMessage = res.error;
                    return st;
                }
            }
        }
        // not found -> assume completed or unknown
        st.state = LoadState::Unknown;
        return st;
    }

	// 名前からテクスチャオブジェクトを取得（存在しなければ nullptr）
    Texture* texture_manager::GetTextureByName(const std::string& name) noexcept
    {
        std::lock_guard<std::mutex> lk(g_texturesMutex);
        auto it = m_Textures.find(name);
        if (it == m_Textures.end()) return nullptr;
        return &it->second;
    }

	// 全テクスチャ名のリストを返す
    std::vector<std::string> texture_manager::GetTextureKeys() const noexcept
    {
        std::vector<std::string> keys;
        std::lock_guard<std::mutex> lk(g_texturesMutex);
        keys.reserve(m_Textures.size());
        for (const auto& kv : m_Textures) keys.push_back(kv.first);
        return keys;
    }

	// 全テクスチャ名とオブジェクトのマップを返す
    const std::unordered_map<std::string, Texture>& texture_manager::GetTextureNames() const noexcept
    {
        return m_Textures;
    }

	// 名前で指定してテクスチャを解放
    bool texture_manager::UnloadTexture(const std::string& name) noexcept
    {
        std::lock_guard<std::mutex> lk(g_texturesMutex);
        auto it = m_Textures.find(name);
        if (it == m_Textures.end()) return false;
        ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(it->second.tx_id);
        if (srv) srv->Release();
        m_Textures.erase(it);
        return true;
    }

	// 全テクスチャを解放
    void texture_manager::UnloadAll() noexcept
    {
        std::lock_guard<std::mutex> lk(g_texturesMutex);
        for (auto& kv : m_Textures) {
            ID3D11ShaderResourceView* srv = reinterpret_cast<ID3D11ShaderResourceView*>(kv.second.tx_id);
            if (srv) srv->Release();
        }
        m_Textures.clear();
    }

	// ロード完了通知コールバックを設定
    void texture_manager::SetLoadCallback(LoadCallback cb)
    {
        g_loadCallback = std::move(cb);
    }


	// テクスチャディレクトリ解決（ヘッダ側で公開している Initialize(baseDir) を使う
    std::filesystem::path texture_manager::ResolveTexturesDir() const
    {
        // 既存の ResolveTexturesDir と同等のロジックをここに移植しても良い
        // ただしヘッダ側で公開している Initialize(baseDir) を使うことを推奨
        // ここでは空を返す（呼び出し側で Initialize を使う想定）
        return {};
    }


	// SRV からテクスチャを登録 srv の所有は texture_manager が保持する
    void texture_manager::RegisterTextureFromSRV(
        const std::string& name,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
        int width, int height)
    {
        if (!srv) return;
        std::lock_guard lk(mutex_);

        // SRV の所有を保持（寿命確保）
        srv_map[name] = srv;

        // 既存の Texture オブジェクトがあれば更新、なければ新規作成して登録
        auto it = textures_.find(name);
        if (it == textures_.end()) {
            auto t = std::make_unique<Texture>();
            t->name = name;
            t->width = width;
            t->height = height;

            ID3D11ShaderResourceView* rawSrv = srv.Get();
            if (rawSrv) {
                if constexpr (std::is_pointer_v<ImTextureID>) {
                    // ImTextureID がポインタ型
                    t->tx_id = reinterpret_cast<ImTextureID>(rawSrv);
                }
                else {
                    // ImTextureID が整数型の場合
                    t->tx_id = static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(rawSrv));
                }
            }
            else {
                t->tx_id = 0;
            }

            textures_[name] = std::move(t);
        }
        else {
            Texture* t = it->second.get();
            t->width = width;
            t->height = height;

            ID3D11ShaderResourceView* rawSrv = srv.Get();
            if (rawSrv) {
                if constexpr (std::is_pointer_v<ImTextureID>) {
                    t->tx_id = reinterpret_cast<ImTextureID>(rawSrv);
                }
                else {
                    t->tx_id = static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(rawSrv));
                }
            }
            else {
                t->tx_id = 0;
            }
        }
    }


    // 戻り値は SRV（ComPtr）を返す。device はメインスレッドの ID3D11Device*
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_manager::CreateSRVFromMemoryStagingCopy(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        const uint8_t* pixels, int width, int height)
    {
        if (!device || !context || !pixels || width <= 0 || height <= 0) return nullptr;

		// GPU テクスチャを作成
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texDefault;
        HRESULT hr = device->CreateTexture2D(&desc, nullptr, texDefault.GetAddressOf());
        if (FAILED(hr) || !texDefault) {
            // 作成失敗
            return nullptr;
        }

        // STAGING テクスチャ（CPU -> GPU のアップロード用）
        D3D11_TEXTURE2D_DESC descStaging = desc;
        descStaging.Usage = D3D11_USAGE_STAGING;
        descStaging.BindFlags = 0;
        descStaging.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texStaging;
        hr = device->CreateTexture2D(&descStaging, nullptr, texStaging.GetAddressOf());
        if (FAILED(hr) || !texStaging) {
            return nullptr;
        }

        // Map -> memcpy -> Unmap
        D3D11_MAPPED_SUBRESOURCE msr;
        hr = context->Map(texStaging.Get(), 0, D3D11_MAP_WRITE, 0, &msr);
        if (FAILED(hr)) {
            return nullptr;
        }

        // 行ごとのコピー（pitch を考慮）
        const int srcRowPitch = width * 4;
        for (int y = 0; y < height; ++y) {
            memcpy(static_cast<uint8_t*>(msr.pData) + (size_t)y * msr.RowPitch,
                pixels + (size_t)y * srcRowPitch,
                srcRowPitch);
        }
        context->Unmap(texStaging.Get(), 0);

        // GPU コマンドとしてコピー（非同期に実行される）
        // PIX に Copy として見える箇所
        context->CopyResource(texDefault.Get(), texStaging.Get());

        // SRV を作る
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        hr = device->CreateShaderResourceView(texDefault.Get(), &srvDesc, srv.GetAddressOf());
        if (FAILED(hr)) {
            return nullptr;
        }

        return srv;

    }


	// ピクセルデータから GPU テクスチャを作成して outTex に設定
    bool texture_manager::CreateGpuTextureFromMemory(const unsigned char* data, int w, int h, Texture& outTex)
    {
        if (!data || w <= 0 || h <= 0) return false;
        ID3D11Device* dev = n_render::Render_GetDevice();
        if (!dev) {
            std::cerr << "[TextureManager] CreateGpuTextureFromMemory: Render_GetDevice() returned null\n";
            return false;
        }

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = static_cast<UINT>(w);
        desc.Height = static_cast<UINT>(h);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA init = {};
        init.pSysMem = data;
        init.SysMemPitch = static_cast<UINT>(w * 4);

        ID3D11Texture2D* tex2d = nullptr;
        HRESULT hr = dev->CreateTexture2D(&desc, &init, &tex2d);
        if (FAILED(hr) || !tex2d) {
            std::cerr << "[TextureManager] CreateTexture2D failed: 0x" << std::hex << hr << std::dec << "\n";
            return false;
        }

        ID3D11ShaderResourceView* srv = nullptr;
        hr = dev->CreateShaderResourceView(tex2d, nullptr, &srv);
        tex2d->Release();
        if (FAILED(hr) || !srv) {
            std::cerr << "[TextureManager] CreateShaderResourceView failed: 0x" << std::hex << hr << std::dec << "\n";
            return false;
        }

        outTex.tx_id = (ImTextureID)srv;
        return true;
    }

	// 結果キューに結果をプッシュ（ムーブ）
    static void PushResult(LoadResult&& res)
    {
        std::lock_guard<std::mutex> lk(g_resMutex);
        g_resQueue.push_back(std::move(res));
    }

	// ワーカースレッドの開始/停止
    void StartWorker() 
    {
        g_workerRunning.store(true);
        g_workerThread = std::thread(WorkerThreadFunc);
    }

	// ワーカースレッドの停止
    void StopWorker() 
    {
        g_workerRunning.store(false);
        g_reqCv.notify_all();
        if (g_workerThread.joinable()) g_workerThread.join();
    }


	// 結果キューからすべての結果をポップ（ムーブ）
    static std::deque<LoadResult> PopAllResults()
    {
        std::deque<LoadResult> out;
        {
            std::lock_guard<std::mutex> lk(g_resMutex);
            out.swap(g_resQueue);
        }
        return out;
    }


	// ワーカースレッド関数
    static void WorkerThreadFunc()
    {
        g_workerRunning.store(true);
        while (g_workerRunning.load()) {
            std::unique_ptr<n_texturemanager::LoadRequest> req;
            {
                std::unique_lock<std::mutex> lk(g_reqMutex);
                g_reqCv.wait(lk, [] { return !g_reqQueue.empty() || !g_workerRunning.load(); });
                if (!g_workerRunning.load() && g_reqQueue.empty()) break;
                req = std::move(g_reqQueue.front());
                g_reqQueue.pop_front();
            }

            if (!req) continue;
            if (req->cancel.load()) continue;

            n_texturemanager::LoadResult res;
            res.id = req->id;
            res.name = req->path.filename().string();

            int w, h, n;
            unsigned char* data = stbi_load(req->path.string().c_str(), &w, &h, &n, 4);
            if (!data) {
                res.success = false;
                res.error = "failed to load";
            }
            else {
                res.success = true;
                res.width = w;
                res.height = h;
                size_t size = static_cast<size_t>(w) * h * 4;
                res.pixels.assign(data, data + size);
                stbi_image_free(data);
            }

            if (req->cancel.load()) {
                continue;
            }

            // Push result (ムーブ)
            PushResult(std::move(res));
        }
    }

}