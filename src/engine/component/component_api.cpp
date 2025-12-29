#include "include/game.h"
#include "include/component/component_api.h"
#include "include/component/component_manager.hpp"
#include "include/window_editor/window_scene.h"
#include "include/component/game_component.h"
#include <optional>

EntityId g_selectedEntity = -1;

bool g_transformConfigOpen = false;
EntityId g_transformConfigEntity = -1;

bool g_moveConfigOpen = false;
EntityId g_moveConfigEntity = -1;

bool g_rigidbodyConfigOpen = false;
EntityId g_rigidbodyConfigEntity = -1;

bool g_isplayerConfigOpen = false;
EntityId g_isplayerConfigEntity = -1;

bool g_startConfigOpen = false;
EntityId g_startConfigEntity = -1;

bool g_finishConfigOpen = false;
EntityId g_finishConfigEntity = -1;

bool g_lightConfigOpen = false;
EntityId g_lightConfigEntity = -1;

// 内部ストレージ（簡易実装）

std::unordered_set<EntityId> g_entitySet;
std::mutex g_entitySetMutex;
std::unordered_set<EntityId> g_dirtyTransforms;



/*n_compomanager::EntityId は Int64_t型のID */

// Has->Get->Set->Add->Remove->Open

namespace n_compoapi
{

    // Transform
    bool HasTransformComponent(EntityId eid) noexcept
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::TransformComponent>(eid);
    }

    std::optional<n_component::TransformComponent> GetTransformComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::TransformComponent>(eid);
    }

    void SetTransformComponent(EntityId eid, const n_component::TransformComponent& t)
    {
        //fprintf(stderr, "[SetTransformComponent] ENTRY eid=%lld thread_hash=%zu saved_hash=%zu addr=%p equal=%d\n",
        //    (long long)eid,
        //    (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()),
        //    (size_t)std::hash<std::thread::id>{}(g_gameThreadId),
        //    (void*)&g_gameThreadId,
        //    (int)(std::this_thread::get_id() == g_gameThreadId));
        //fflush(stderr);
        // ゲームスレッド前提。誤って別スレッドから呼ばれたらログを出して早期 return
        if (!IsGameThread()) {
            n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, t]() {
                SetTransformComponentImpl(eid, t);
                });
            fprintf(stderr, "[SetTransformComponent] forwarded to game thread (fallback) eid=%lld\n", (long long)eid);
            fflush(stderr);
            return;
        }
        SetTransformComponentImpl(eid, t);
    }

    // SetTransformComponentのヘルプ関数
    void SetTransformComponentImpl(EntityId eid, const n_component::TransformComponent& t)
    {
        bool changed = true;
        if (auto oldOpt = n_compoapi::GetTransformComponent(eid))
        {
            const auto& old = *oldOpt;
            if (old.position[0] == t.position[0] && old.position[1] == t.position[1]
                && old.rotation == t.rotation && old.scale == t.scale)
            {
                changed = false;
            }
        }

        // ゲームスレッドでストレージを書き換える（ここに移動）
        n_compomanager::g_componentManager.SetComponent<n_component::TransformComponent>(eid, t);

        if (!changed) return;

        // スプライト同期処理（ゲームスレッド上で安全に行う）
        auto spOpt = n_compoapi::GetSpriteComponent(eid);
        if (spOpt) {
            const float px = t.position[0];
            const float py = t.position[1];
            std::array<float, 2> pos = { px, py };

            Vec2 cur;
            bool hasPos = n_windowscene::instance_winSce().GetSpritePosition(eid, cur);

            const float EPS = 1e-5f;
            // Vec2 のアクセスは .x/.y を使う実装に合わせてください
            bool needSync = !hasPos || (fabsf(cur[0] - px) > EPS || fabsf(cur[1] - py) > EPS);

            if (needSync) {
                SceneSprite s;
                s.id = spOpt->spriteId;
                s.selected = spOpt->visible;
                n_compoapi::RegisterSpriteAndSyncTransform(eid, s, pos);
            }
        }
    }

    void AddTransformComponent(EntityId eid)
    {
        // 既に持っていれば何もしない（ログを残す）
        if (n_compomanager::g_componentManager.HasComponent<n_component::TransformComponent>(eid)) {
            printf("[AddMoveComponent/既に持っていれば]: already has Move for %lld\n", (long long)eid);
            return;
        }

        n_component::TransformComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::TransformComponent>(eid, def);

        EnsureEntityRegistered(eid);

        // 値キャプチャはムーブキャプチャにすると効率的（C++14 以降）
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def = std::move(def)]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenTransformConfigFor(eid);
    }

    void RemoveTransformComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::TransformComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenTransformConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_transformConfigEntity = eid;
        g_transformConfigOpen = true;
    }



    // Move
    bool HasMoveComponent(EntityId eid)  noexcept
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::MoveComponent>(eid);
    }

    std::optional<n_component::MoveComponent> GetMoveComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::MoveComponent>(eid);
    }

    void SetMoveComponent(EntityId eid, const n_component::MoveComponent& m)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::MoveComponent>(eid, m);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, m]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddMoveComponent(EntityId eid)
    {
        // 既に持っていれば何もしない（ログを残す）
        if (n_compomanager::g_componentManager.HasComponent<n_component::MoveComponent>(eid)) {
            printf("[AddMoveComponent/既に持っていれば]: already has Move for %lld\n", (long long)eid);
            return;
        }

        n_component::MoveComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::MoveComponent>(eid, def);

        EnsureEntityRegistered(eid);

        // 値キャプチャはムーブキャプチャにすると効率的（C++14 以降）
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def = std::move(def)]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenMoveConfigFor(eid);
    }

    void RemoveMoveComponent(EntityId eid)
    {
        if (!n_compomanager::g_componentManager.HasComponent<n_component::MoveComponent>(eid)) {
            printf("RemoveMoveComponent: no Move for %lld\n", (long long)eid);
            return;
        }
        n_compomanager::g_componentManager.RemoveComponent<n_component::MoveComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 要に応じてランタイムを追加
            });
    }

    void OpenMoveConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_moveConfigEntity = eid;
        g_moveConfigOpen = true;
    }



    // IsPlayer
    bool HasIsPlayerComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::IsPlayerComponent>(eid);
    }

    std::optional<n_component::IsPlayerComponent> GetIsPlayerComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::IsPlayerComponent>(eid);
    }

    void SetIsPlayerComponent(EntityId eid, const n_component::IsPlayerComponent& IsPlayer)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::IsPlayerComponent>(eid, IsPlayer);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, IsPlayer]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddIsPlayerComponent(EntityId eid)
    {
        n_component::IsPlayerComponent def{}; // ← 正しい型
        n_compomanager::g_componentManager.AddComponent<n_component::IsPlayerComponent>(eid, def);
        // エンティティ集合に登録（未登録なら）
        {
            EnsureEntityRegistered(eid);
        }
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenIsPlayerConfigFor(eid);
    }

    void RemoveIsPlayerComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::IsPlayerComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenIsPlayerConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_isplayerConfigEntity = eid;
        g_isplayerConfigOpen = true;
    }



    // Rigidbody
    bool HasRigidbodyComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::RigidbodyComponent>(eid);
    }

    std::optional<n_component::RigidbodyComponent> GetRigidbodyComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::RigidbodyComponent>(eid);
    }

    void SetRigidbodyComponent(EntityId eid, const n_component::RigidbodyComponent& rb)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::RigidbodyComponent>(eid, rb);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, rb]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddRigidbodyComponent(EntityId eid)
    {
        EnsureEntityRegistered(eid);
        n_component::RigidbodyComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::RigidbodyComponent>(eid, def);
        // エンティティ集合に登録（未登録なら）
        {
            EnsureEntityRegistered(eid);
        }
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenRigidbodyConfigFor(eid);
    }

    void RemoveRigidbodyComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::RigidbodyComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenRigidbodyConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_rigidbodyConfigEntity = eid;
        g_rigidbodyConfigOpen = true;
    }



    // Start
    bool HasStartComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::StartComponent>(eid);
    }

    std::optional<n_component::StartComponent> GetStartComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::StartComponent>(eid);
    }

    void SetStartComponent(EntityId eid, const n_component::StartComponent& s)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::StartComponent>(eid, s);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, s]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddStartComponent(EntityId eid)
    {
        EnsureEntityRegistered(eid);
        n_component::StartComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::StartComponent>(eid, def);
        // エンティティ集合に登録（未登録なら）
        {
            EnsureEntityRegistered(eid);
        }
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenStartConfigFor(eid);
    }

    void RemoveStartComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::StartComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenStartConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_startConfigEntity = eid;
        g_startConfigOpen = true;
    }



    // Finish
    bool HasFinishComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::FinishComponent>(eid);
    }

    std::optional<n_component::FinishComponent> GetFinishComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::FinishComponent>(eid);
    }

    void SetFinishComponent(EntityId eid, const n_component::FinishComponent& f)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::FinishComponent>(eid, f);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, f]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddFinishComponent(EntityId eid)
    {
        EnsureEntityRegistered(eid);
        n_component::FinishComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::FinishComponent>(eid, def);
        // エンティティ集合に登録（未登録なら）
        {
            EnsureEntityRegistered(eid);
        }
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenFinishConfigFor(eid);
    }

    void RemoveFinishComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::FinishComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenFinishConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_finishConfigEntity = eid;
        g_finishConfigOpen = true;
    }



    // Light
    bool HasLightComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::LightComponent>(eid);
    }

    std::optional<n_component::LightComponent> GetLightComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.GetComponent<n_component::LightComponent>(eid);
    }

    void SetLightComponent(EntityId eid, const n_component::LightComponent& l)
    {
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::LightComponent>(eid, l);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, l]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
    }

    void AddLightComponent(EntityId eid)
    {
        EnsureEntityRegistered(eid);
        n_component::LightComponent def{};
        n_compomanager::g_componentManager.AddComponent<n_component::LightComponent>(eid, def);
        // エンティティ集合に登録（未登録なら）
        {
            EnsureEntityRegistered(eid);
        }
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, def]() {
            // ランタイム側の追加処理（必要なら）
            });
        OpenLightConfigFor(eid);
    }

    void RemoveLightComponent(EntityId eid)
    {
        n_compomanager::g_componentManager.RemoveComponent<n_component::LightComponent>(eid);
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid]()
            {
                // 必要に応じてランタイムを追加
            });
    }

    void OpenLightConfigFor(EntityId eid)
    {
        g_selectedEntity = eid;
        g_lightConfigEntity = eid;
        g_lightConfigOpen = true;
    }

    /* Spriteの処理は少ないため、ヘッダにある */


    // Spriteの初期配置とTransform
    void RegisterSpriteAndSyncTransform(EntityId eid, const SceneSprite& sprite, const std::array<float, 2>& pos)
    {
        //fprintf(stderr, "[RegisterSpriteAndSyncTransform] ENTRY eid=%lld spriteId=%lld pos=(%f,%f) thread=%zu\n",
        //    (long long)eid, (long long)sprite.id, pos[0], pos[1],
        //    (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
        //fflush(stderr);

        // Register は必ず呼ぶ
        n_windowscene::instance_winSce().RegisterSprite(eid, sprite);

        // 位置は呼び出し側から受け取るので GetTransformComponent に依存しない
        bool ok = n_windowscene::instance_winSce().SetSpritePosition(eid, pos);
        if (!ok) {
            // retry with sprite.id if API expects that
            ok = n_windowscene::instance_winSce().SetSpritePosition(sprite.id, pos);
        }
        if (!ok) {
            n_game::instance_game().MarkTransformDirty(eid);
            printf("[RegisterSpriteAndSyncTransform] fallback: marked dirty eid=%lld\n", (long long)eid);
        }
        // printf("[RegisterSpriteAndSyncTransform] eid=%lld pos=(%f,%f) ok=%d\n", (long long)eid, pos[0], pos[1], ok);
        return;
    }

    bool HasSpriteComponent(EntityId eid)
    {
        return n_compomanager::g_componentManager.HasComponent<n_component::SpriteComponent>(eid);
    }


    std::optional<n_component::SpriteComponent> GetSpriteComponent(EntityId eid) {
        return n_compomanager::g_componentManager.GetComponent<n_component::SpriteComponent>(eid);
    }


    void InitializeGameThreadId()
    {
        bool expected = false;
        // 既に初期化済みなら上書きしない
        if (!g_gameThreadIdInitialized.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
            fprintf(stderr, "[InitializeGameThreadId] WARNING: already initialized saved_hash=%zu addr=%p\n",
                (size_t)std::hash<std::thread::id>{}(g_gameThreadId), (void*)&g_gameThreadId);
            fflush(stderr);
            return;
        }
        g_gameThreadId = std::this_thread::get_id();
        fprintf(stderr, "[InitializeGameThreadId] saved hash=%zu addr=%p\n",
            (size_t)std::hash<std::thread::id>{}(g_gameThreadId), (void*)&g_gameThreadId);
        fflush(stderr);
    }


    bool IsGameThread()
    {
        //fprintf(stderr, "[DBG] g_gameThreadId current hash=%zu addr=%p\n",
        //    (size_t)std::hash<std::thread::id>{}(g_gameThreadId), (void*)&g_gameThreadId);
        //fflush(stderr);
        if (!g_gameThreadIdInitialized.load(std::memory_order_acquire)) {
            return false; // 初期化前は false を返す（ログで検出しやすくする）
        }
        return std::this_thread::get_id() == g_gameThreadId;
    }


    void EnsureEntityRegistered(EntityId eid) {
        std::lock_guard<std::mutex> lk(g_entitySetMutex);
        g_entitySet.insert(eid);
    }

    // 全てのオブジェクトを取得
    std::vector<EntityId> GetAllEntities()
    {
        std::lock_guard<std::mutex> lk(g_entitySetMutex);
        return std::vector<EntityId>(g_entitySet.begin(), g_entitySet.end());
    }
}