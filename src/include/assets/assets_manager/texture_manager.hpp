/*テクスチャの読み込みを行う.png/.jpeg*/

#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <functional>
#include <wrl.h>
#include <d3d11.h>
#include <mutex>
#include <atomic>

#include "include/assets/texture.h"

namespace n_texturemanager
{
	using RequestId = uint64_t;

    // 非同期要求を表す型（ヘッダに置くことで cpp と一致させる）
    struct LoadRequest 
    {
        RequestId id = 0;
        std::filesystem::path path;
        std::atomic<bool> cancel{ false };

        // コピー禁止
        LoadRequest(const LoadRequest&) = delete;
        LoadRequest& operator=(const LoadRequest&) = delete;

        // 明示的なムーブコンストラクタ / ムーブ代入（std::atomic はムーブ不可なので手動で扱う）
        LoadRequest(LoadRequest&& other) noexcept
            : id(other.id)
            , path(std::move(other.path))
        {
            // atomic はムーブできないので値をコピーしておく
            bool v = other.cancel.load();
            cancel.store(v);
            other.cancel.store(false);
        }

        LoadRequest& operator=(LoadRequest&& other) noexcept
        {
            if (this != &other) {
                id = other.id;
                path = std::move(other.path);
                bool v = other.cancel.load();
                cancel.store(v);
                other.cancel.store(false);
            }
            return *this;
        }

        LoadRequest() = default;
        
    };

	enum class LoadState {Unknown, Queued, Loading, ReadyToFinalize, Completed, Failed, Cancelled};

    struct LoadStatus
    {
        RequestId id = 0;
        LoadState state = LoadState::Unknown;
        float progress = 0.0f; // 0.0〜1.0
        std::string name;	// 最終的に読み込まれたテクスチャ名
        std::string errorMessage; // エラー発生時のメッセージ
    };

    struct LoadResult
    {
        uint32_t id;
        std::string name;
        int width;
        int height;
        std::vector<uint8_t> pixels; // RGBA
        bool success;
        std::string error;
    };

	class texture_manager
    {
        public:
            texture_manager() = default;
            ~texture_manager();


            // ライフサイクル
            bool Initialize(const std::filesystem::path& baseDir); // baseDir を設定（起動時1回）
            void Shutdown(); // ワーカー停止、リソース解放


            // 同期ロード（従来互換）
            size_t LoadAllTextures(); // baseDir 配下を列挙して同期ロード（戻り値: 読込数）
            bool LoadTextureFromFile(const std::filesystem::path& filepath); // ブロッキング


            // 非同期ロード API（将来の切替用）
            RequestId LoadTextureAsync(const std::filesystem::path& filepath); // 即時 RequestId を返す
            static void EnqueueRequest(LoadRequest&& req);    // 要求の追加を行う
            bool CancelLoad(uint32_t id); // キャンセル要求
            void ProcessPendingResults(); // メインスレッドで呼び、GPU 作成と登録を行う
            LoadStatus GetLoadStatus(RequestId id) const;


            // 登録API
            void RegisterTextureFromSRV(const std::string& name,
                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
                int width, int height);
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateSRVFromMemoryStagingCopy(
                ID3D11Device* device,
                ID3D11DeviceContext* context,
                const uint8_t* pixels, int width, int height);



            // 取得/列挙/解放
            Texture* GetTextureByName(const std::string& name) noexcept;
            std::vector<std::string> GetTextureKeys() const noexcept;
            const std::unordered_map<std::string, Texture>& GetTextureNames() const noexcept;


            bool UnloadTexture(const std::string& name) noexcept;
            void UnloadAll() noexcept;


            // 通知コールバック（UI 用、メインスレッドで呼ばれる）
            using LoadCallback = std::function<void(const LoadStatus&)>;
            void SetLoadCallback(LoadCallback cb); // 完了/失敗時に呼ばれる（メインスレッド）

        private:
            // 内部ユーティリティ（実装ファイルで定義）
            std::filesystem::path ResolveTexturesDir() const;
            bool CreateGpuTextureFromMemory(const unsigned char* data, int w, int h, Texture& outTex);

            // 非同期用内部構造は cpp に隠蔽
            std::filesystem::path m_baseDir;
            std::unordered_map<std::string, Texture> m_Textures;

            std::unordered_map<std::string, std::unique_ptr<Texture>> textures_; // 既存
            std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv_map; // 新規（所有）
            std::mutex mutex_;
    };

	// グローバルインスタンス
	texture_manager& instance_texmag();
}

#endif // !TEXTURE_MANAGER

/*
Initialize(baseDir)
m_baseDir を設定し、ワーカースレッド群を起動する（非同期対応時）。存在チェックやログを行う。

Shutdown()
ワーカー停止、結果キューの処理、GPU リソース（SRV）の Release、m_Textures クリア。

LoadAllTextures()
m_baseDir を列挙して LoadTextureFromFile() を同期で呼ぶ。互換性維持用。非同期移行時は内部で LoadTextureAsync を使って ProcessPendingResults() を待つラッパーにできる。

LoadTextureFromFile(filepath)
stbi_load → CreateGpuTextureFromMemory → m_Textures に登録。ブロッキング。エラーは false を返す。

LoadTextureAsync(filepath)
リクエストをキューに入れて RequestId を返す。ワーカーがファイル読み込み・デコードを行い、結果を結果キューに push。

ProcessPendingResults()
メインスレッドで呼ぶ。結果キューから ImageData を取り出し CreateGpuTextureFromMemory を呼んで m_Textures に登録。登録後は LoadCallback を呼ぶ。

GetLoadStatus(id)
リクエストの状態を返す（Queued/Loading/ReadyToFinalize/Completed/Failed/Cancelled）。UI の進捗表示に使う。

GetTextureByName / GetTextureKeys / GetTextureNames
参照用。Texture* は texture_manager が所有する SRV を指す。外部は長期保持しないか、無効化に注意。

UnloadTexture / UnloadAll
SRV を Release して m_Textures から削除。UI 側へ通知する場合は LoadCallback とは別にコールバックを用意しても良い。

*/