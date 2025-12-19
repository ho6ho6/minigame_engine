/********************************************
 *input::Update, IsKeyDown, マウス処理の定義*
 ********************************************/

#include "include/input.hpp"
#include "include/game.hpp"
#include "include/component/component_api.hpp"
#include <Windows.h>
#include <unordered_map>
#include <include/component/component_manager.hpp>
static std::unordered_map<int64_t, bool> s_wasJumpPressed;

 /*入力*/
namespace n_input
{
    void UpdateInputAndJumpForAll() {
        for (int64_t entityId : n_compoapi::GetAllEntities()) {
            auto mcOpt = n_compoapi::GetMoveComponent(entityId);
            if (!mcOpt) continue;
            auto mc = *mcOpt; // コピー

            if (mc.jumpKey < 0) {
                s_wasJumpPressed.erase(entityId);
                continue;
            }

            SHORT state = GetAsyncKeyState(mc.jumpKey);
            bool isDown = (state & 0x8000) != 0;
            bool wasDown = s_wasJumpPressed[entityId];

            if (isDown && !wasDown) {
                if (n_game::instance_game().IsGrounded(entityId)) {
                    // 高さ方式を採るなら:
                    n_game::instance_game().ApplyJumpImpulse(entityId, mc.jump);

                    // もし MoveComponent 側に何か状態を残す必要があれば更新する
                    // 例: mc.lastJumpTime = currentTime;
                    // n_compoapi::SetMoveComponent(entityId, mc);
                }
            }
            s_wasJumpPressed[entityId] = isDown;
        }
    }

    void PhysicsStep(float dt)
    {
        // dt が極端に大きい/小さいと不安定になるので必要なら clamp する
        if (dt <= 0.0f) return;
        const float maxDt = 0.05f;
        if (dt > maxDt) dt = maxDt;

        for (int64_t entityId : n_compoapi::GetAllEntities()) {
            auto rbOpt = n_compoapi::GetRigidbodyComponent(entityId);
            if (!rbOpt) continue;
            auto rb = *rbOpt; // コピーして編集

            // 1) 速度に重力を適用（gravity は負の値を想定）
            rb.gravity += rb.gravity * dt;

            // 2) Transform を取得して位置を速度で更新
            auto tOpt = n_compoapi::GetTransformComponent(entityId);
            if (!tOpt) {
                // Transform が無ければ Rigidbody の変更だけ保存して次へ
                n_compoapi::SetRigidbodyComponent(entityId, rb);
                continue;
            }

            auto t = *tOpt; // コピー
            t.position[1] += rb.gravity * dt; // Y 成分を更新

            // 3) 簡易床判定（y <= 0 を床とする）
            if (t.position[1] <= 0.0f) {
                t.position[1] = 0.0f;
                rb.gravity = 0.0f;
                rb.isGround = true;
            }
            else {
                rb.isGround = false;
            }

            // 4) 変更を永続化
            n_compoapi::SetTransformComponent(entityId, t);
            n_compoapi::SetRigidbodyComponent(entityId, rb);
        }
    }

} // namespace Input