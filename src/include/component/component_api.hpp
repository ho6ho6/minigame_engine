#ifndef COMPO_API
#define COMPO_API

#pragma once
#include <cstdint>
#include <optional>
#include "include/component/componentDefaults.hpp"
#include "include/component/component_manager.hpp"
#include <vector>
#include <unordered_set>
/*n_compomanager::EntityId は Int64_t型のID */

using EntityId = n_compomanager::EntityId;
extern EntityId g_selectedEntity;

// move
extern bool g_moveConfigOpen;
extern EntityId g_moveConfigEntity;

// transform
extern bool g_transformConfigOpen;
extern EntityId g_transformConfigEntity;

// isplayer
extern bool g_isplayerConfigOpen;
extern EntityId g_isplayerConfigEntity;

// rigidbody
extern bool g_rigidbodyConfigOpen;
extern EntityId g_rigidbodyConfigEntity;

// start
extern bool g_startConfigOpen;
extern EntityId g_startConfigEntity;

// finish
extern bool g_finishConfigOpen;
extern EntityId g_finishConfigEntity;

// light
extern bool g_lightConfigOpen;
extern EntityId g_lightConfigEntity;

// 選択判定
extern std::unordered_set<EntityId> g_entitySet;
extern std::mutex g_entitySetMutex;


namespace n_compoapi
{
    // 移動に関わるものには noexceptを付与
    // 移動コンストラクタやムーブ演算子に付けると標準ライブラリがムーブを選びやすくなり性能向上につながる らしい

    // Transform コンポーネント
    bool HasTransformComponent(EntityId eid) noexcept;
    std::optional<n_component::TransformComponent> GetTransformComponent(EntityId eid);
    void SetTransformComponent(EntityId eid, const n_component::TransformComponent& t);
    void AddTransformComponent(EntityId eid);
    void RemoveTransformComponent(EntityId eid);
    void OpenTransformConfigFor(EntityId eid);

    // Move コンポーネント
    bool HasMoveComponent(EntityId eid) noexcept;
    std::optional<n_component::MoveComponent> GetMoveComponent(EntityId eid);
    void SetMoveComponent(EntityId eid, const n_component::MoveComponent& m);
    void AddMoveComponent(EntityId eid);
    void RemoveMoveComponent(EntityId eid);
    void OpenMoveConfigFor(EntityId eid);

    // IsPlayer コンポーネント
    bool HasIsPlayerComponent(EntityId eid);
    std::optional<n_component::IsPlayerComponent> GetIsPlayerComponent(EntityId eid);
    void SetIsPlayerComponent(EntityId eid, const n_component::IsPlayerComponent& isplayer);
    void AddIsPlayerComponent(EntityId eid);
    void RemoveIsPlayerComponent(EntityId eid);
    void OpenIsPlayerConfigFor(EntityId eid);

    // Rigidbody コンポーネント
    bool HasRigidbodyComponent(EntityId eid);
    std::optional<n_component::RigidbodyComponent> GetRigidbodyComponent(EntityId eid);
    void SetRigidbodyComponent(EntityId eid, const n_component::RigidbodyComponent& rb);
    void AddRigidbodyComponent(EntityId eid);
    void RemoveRigidbodyComponent(EntityId eid); // typo fixed
    void OpenRigidbodyConfigFor(EntityId eid);

    // Start / Finish (タグ系)
    bool HasStartComponent(EntityId eid);
    std::optional<n_component::StartComponent> GetStartComponent(EntityId eid);
    void SetStartComponent(EntityId eid, const n_component::StartComponent& s);
    void AddStartComponent(EntityId eid);
    void RemoveStartComponent(EntityId eid);
    void OpenStartConfigFor(EntityId eid);

    bool HasFinishComponent(EntityId eid);
    std::optional<n_component::FinishComponent> GetFinishComponent(EntityId eid);
    void SetFinishComponent(EntityId eid, const n_component::FinishComponent& f);
    void AddFinishComponent(EntityId eid);
    void RemoveFinishComponent(EntityId eid);
    void OpenFinishConfigFor(EntityId eid);

    // Light コンポーネント
    bool HasLightComponent(EntityId eid);
    std::optional<n_component::LightComponent> GetLightComponent(EntityId eid);
    void SetLightComponent(EntityId eid, const n_component::LightComponent& l);
    void AddLightComponent(EntityId eid);
    void RemoveLightComponent(EntityId eid);
    void OpenLightConfigFor(EntityId eid);

    // その他
    std::vector<EntityId> GetAllEntities();

    // 既にヘッダで安全に呼べる簡易ラッパー（例）
    inline std::optional<n_component::SpriteComponent> GetSpriteComponent(EntityId eid) {
        return n_compomanager::g_componentManager.GetComponent<n_component::SpriteComponent>(eid);
    }

    // どの AddXComponent でも共通に呼べるヘルパ
    inline void EnsureEntityRegistered(EntityId eid) {
        std::lock_guard<std::mutex> lk(g_entitySetMutex);
        g_entitySet.insert(eid);
    }
}

#endif // !COMPO_API
