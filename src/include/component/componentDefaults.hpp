#ifndef COMPO_DEFAULTS
#define COMPO_DEFAULTS

#pragma once

#include <array>
#include <cstdint>

// 小さな型エイリアスはヘッダで可。必要なら名前空間に入れる。
using Vec2 = std::array<float, 2>;
using Vec3 = std::array<float, 3>;

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
        Vec2 direction{ 0.0f, 0.0f};
        float acceleration = 0.0f;
        float jump = 0.0f;
        int jumpKey = 32; // デフォルト Space のキーコード（環境に合わせて変更）
    };
    struct MoveComponent {
        float speed = 1.0f;
        Vec2 direction{ 0.0f, 0.0f};
        float acceleration = 0.0f;
        float jump = 0.0f;
        int jumpKey = 32; // 初期値を必ず与える
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
        float gravity = -1.0f;
        bool isGround = false;
    };
    struct RigidbodyComponent {
        float gravity = -1.0f;
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
        int64_t PlayerId = -1;
        int64_t EntityId = -2;
    };
    struct IsPlayerComponent {
        bool IsPlayerActive = false;
        bool IsEnemyActive = false;
        int64_t PlayerId = -1;
        int64_t EntityId = -2;
    };


    struct SpriteDefaults {
        int64_t spriteId = -1;
        bool visible = true;
        // 必要なら layer, uv など
    };
    struct SpriteComponent {
        int64_t spriteId = -1;
        bool visible = true;
        // 必要なら layer, uv など
    };

} // namespace components
#endif // !COMPO_DEFAULTS