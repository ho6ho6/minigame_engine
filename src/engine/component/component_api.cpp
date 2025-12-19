#include "include/component/component_api.hpp"
#include "include/component/component_manager.hpp"
#include "include/component/game_component.hpp"
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
        // キャッシュ更新（即時UI反映）
        n_compomanager::g_componentManager.SetComponent<n_component::TransformComponent>(eid, t);
        // ゲーム側に反映する必要があるなら非同期コマンドを投げる
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, t]()
            {
                // ゲームスレッド: 別個の場合は実行時シーンに適用
            });
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

    // Start/Finish
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


    // 全てのオブジェクトを取得
    std::vector<EntityId> GetAllEntities()
    {
        std::lock_guard<std::mutex> lk(g_entitySetMutex);
        return std::vector<EntityId>(g_entitySet.begin(), g_entitySet.end());
    }

}