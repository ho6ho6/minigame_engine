#ifndef COMPONENT_DEFAULTS_H
#define COMPONENT_DEFAULTS_H

#include <array>
#include <cstdint>
#include "imgui.h"

// 小さな型エイリアスはヘッダで可。必要なら名前空間に入れる。
using Vec2 = std::array<float, 2>;
using Vec3 = std::array<float, 3>;
using EntityId = int64_t;

// デフォルト値とコンポーネント本体を同じヘッダに置く場合の安全な実装
namespace n_component {

    // Transform
    struct TransformDefaults {
        Vec2 position{ 0.0f, 0.0f};
        Vec2 rotation{ 0.0f, 0.0f};
        Vec2 scale{ 1.0f, 1.0f};
    };
    struct TransformComponent {
        Vec2 position{ 0.0f, 0.0f};
        Vec2 rotation{ 0.0f, 0.0f};
        Vec2 scale{ 1.0f, 1.0f};
    };

    // Move
    struct MoveDefaults {
        float speed = 1.0f;
        Vec2 direction{ 1.0f, 1.0f};
        float acceleration = 0.0f;
        float jump = 1.0f;
        ImGuiKey jumpKey = ImGuiKey_Space;
        ImGuiKey directionUpKey = ImGuiKey_W;
        ImGuiKey directionRightKey = ImGuiKey_D;
        ImGuiKey directionLeftKey = ImGuiKey_A;
        ImGuiKey directionDownKey = ImGuiKey_S;
    };
    struct MoveComponent {
        float speed = 1.0f;
        Vec2 direction{ 1.0f, 1.0f};
        float acceleration = 0.0f;
        float jump = 1.0f;
        ImGuiKey jumpKey = ImGuiKey_Space;
        ImGuiKey directionUpKey = ImGuiKey_W;
        ImGuiKey directionRightKey = ImGuiKey_D;
        ImGuiKey directionLeftKey = ImGuiKey_A;
        ImGuiKey directionDownKey = ImGuiKey_S;
    };

    // Light
    struct LightDefaults {
        Vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float range = 10.0f;
    };
    struct LightComponent {
        Vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float range = 10.0f;
    };

    // Gravity
    struct RigidbodyDefaults {
        Vec2 Vel = {0.0f, 0.0f};
        Vec2 gravity = { 0.0f, -9.8f };
        float jumpElapsed = 0.0f;   // 経過時間
        bool isJump = false;
        bool isGround = false;
    };
    struct RigidbodyComponent {
        Vec2 Vel = { 0.0f, 0.0f };
        Vec2 gravity = { 0.0f, -9.8f };
        float jumpElapsed = 0.0f;
        bool isJump = false;
        bool isGround = false;
    };

    // Start
    struct StartDefaults {
        float spawnRadius = 0.0f;
        int priority = 0;
        bool active = true;
    };
    struct StartComponent {
        float spawnRadius = 0.0f;
        int priority = 0;
        bool active = true;
    };

    // Finish
    struct FinishDefaults {
        float radius = 1.0f;
        bool oneShot = true;
        bool active = true;
    };
    struct FinishComponent {
        float radius = 1.0f;
        bool oneShot = true;    //一回だけ処理される　（クリア判定、イベント起点、エフェクト再生などに）
        bool active = true;
    };

    // IsPlayer
    struct IsPlayerDefaults {
        bool IsPlayerActive = false;
        bool IsEnemyActive = false;
        EntityId PlayerId = 1;
        EntityId EntityId = -2;
    };
    struct IsPlayerComponent {
        bool IsPlayerActive = false;
        bool IsEnemyActive = false;
        EntityId PlayerId = 1;
        EntityId EntityId = -2;
    };


    struct SpriteDefaults {
        EntityId spriteId = 1;
        bool visible = true;
        // 必要なら layer, uv など
    };
    struct SpriteComponent {
        EntityId spriteId = 1;
        bool visible = true;
        // 必要なら layer, uv 
    };

}

#endif // COMPONENT_DEFAULTS_H