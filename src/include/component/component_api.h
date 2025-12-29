#ifndef COMPO_API
#define COMPO_API

#pragma once
#include <cstdint>
#include <optional>
#include "include/component/componentDefaults.h"
#include "include/component/component_manager.hpp"
#include "include/window_editor/window_scene_sprite.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
/*n_compomanager::EntityId は Int64_t型のID */

using EntityId = n_compomanager::EntityId;
extern EntityId g_selectedEntity;

// move
extern bool g_moveConfigOpen;
extern EntityId g_moveConfigEntity;

static std::mutex g_moveMutex;
static std::unordered_map<EntityId, n_component::MoveComponent> g_moveStore;
static std::unordered_map<EntityId, n_component::MoveComponent> g_prevMoveStore; // 補間用
static std::unordered_set<EntityId> g_dirtyMove;

// transform
extern bool g_transformConfigOpen;
extern EntityId g_transformConfigEntity;
// グローバル／静的（ヘッダ側）
static std::mutex g_transformMutex;
static std::unordered_map<EntityId, n_component::TransformComponent> g_transformStore;
static std::unordered_map<EntityId, n_component::TransformComponent> g_prevTransformStore; // 補間用
static std::mutex g_dirtyMutex;
extern std::unordered_set<EntityId> g_dirtyTransforms;

// isplayer
extern bool g_isplayerConfigOpen;
extern EntityId g_isplayerConfigEntity;

static std::mutex g_isplayerMutex;
static std::unordered_map<EntityId, n_component::IsPlayerComponent> g_isplayerStore;
static std::unordered_map<EntityId, n_component::IsPlayerComponent> g_prevIsPlayerStore; // 補間用
static std::unordered_set<EntityId> g_dirtyIsPlayer;

// rigidbody
extern bool g_rigidbodyConfigOpen;
extern EntityId g_rigidbodyConfigEntity;

static std::mutex g_rigidbodyMutex;
static std::unordered_map<EntityId, n_component::RigidbodyComponent> g_rigidbodyStore;
static std::unordered_map<EntityId, n_component::RigidbodyComponent> g_prevRigidbodyStore; // 補間用
static std::unordered_set<EntityId> g_dirtyRigidbody;

// start
extern bool g_startConfigOpen;
extern EntityId g_startConfigEntity;

static std::mutex g_startMutex;
static std::unordered_map<EntityId, n_component::StartComponent> g_startStore;
static std::unordered_map<EntityId, n_component::StartComponent> g_prevStartStore; // 補間用
static std::unordered_set<EntityId> g_dirtyStart;

// finish
extern bool g_finishConfigOpen;
extern EntityId g_finishConfigEntity;

static std::mutex g_finishMutex;
static std::unordered_map<EntityId, n_component::FinishComponent> g_finishStore;
static std::unordered_map<EntityId, n_component::FinishComponent> g_prevFinishStore; // 補間用
static std::unordered_set<EntityId> g_dirtyFinish;

// light
extern bool g_lightConfigOpen;
extern EntityId g_lightConfigEntity;

static std::mutex g_lightMutex;
static std::unordered_map<EntityId, n_component::LightComponent> g_lightStore;
static std::unordered_map<EntityId, n_component::LightComponent> g_prevlightStore; // 補間用
static std::unordered_set<EntityId> g_dirtyLight;

// sprite
extern bool g_spriteConfigOpen;
extern EntityId g_spriteConfigEntity;

// 選択判定
extern std::unordered_set<EntityId> g_entitySet;
extern std::mutex g_entitySetMutex;

// Transformの同期
extern std::mutex g_dirtyMutex;
extern std::unordered_set<EntityId> g_dirtyTransforms;

// gamethreadIDに保存
static std::thread::id g_gameThreadId;
static std::atomic<bool> g_gameThreadIdInitialized{ false };

namespace n_compoapi
{
    // 移動に関わるものには noexceptを付与
    // 移動コンストラクタやムーブ演算子に付けると標準ライブラリがムーブを選びやすくなり性能向上につながる らしい

    // Transform コンポーネント
    bool HasTransformComponent(EntityId eid) noexcept;
    std::optional<n_component::TransformComponent> GetTransformComponent(EntityId eid);
    void SetTransformComponent(EntityId eid, const n_component::TransformComponent& t);
    //  SetTransformComponentのヘルプ関数
    void SetTransformComponentImpl(EntityId eid, const n_component::TransformComponent& t);

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

    // Start (タグ系)
    bool HasStartComponent(EntityId eid);
    std::optional<n_component::StartComponent> GetStartComponent(EntityId eid);
    void SetStartComponent(EntityId eid, const n_component::StartComponent& s);
    void AddStartComponent(EntityId eid);
    void RemoveStartComponent(EntityId eid);
    void OpenStartConfigFor(EntityId eid);

    // Finish
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


    // Sprite コンポーネント
    bool HasSpriteComponent(EntityId eid);
    std::optional<n_component::SpriteComponent> GetSpriteComponent(EntityId eid);


    // sceneに配置されたSpriteの座標をTransformに反映させる
    void RegisterSpriteAndSyncTransform(EntityId eid, const SceneSprite& sprite, const std::array<float, 2>& pos);

    // その他
    std::vector<EntityId> GetAllEntities();

    // 既にヘッダで安全に呼べる簡易ラッパー（例）
    bool HasSpriteComponent(EntityId eid);
    
    std::optional<n_component::SpriteComponent> GetSpriteComponent(EntityId eid);

    void InitializeGameThreadId();

    bool IsGameThread();

    // どの AddXComponent でも共通に呼べるヘルパ
    void EnsureEntityRegistered(EntityId eid);
}

#endif // !COMPO_API
