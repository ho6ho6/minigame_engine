#include "include/input.h"
#include "include/game.h"
#include "include/component/component_api.h"
#include "include/component/componentDefaults.h"
#include <Windows.h>
#include <unordered_map>
#include <include/component/component_manager.hpp>
static std::unordered_map<int64_t, bool> s_wasJumpPressed;

 /*入力*/
namespace n_input
{
    void PollPlayerInputAndEnqueue(int64_t eid)
    {
        // プレイヤーかどうか
        if (!n_compomanager::g_componentManager.HasComponent<n_component::IsPlayerComponent>(eid)) return;
        
        // MoveComponentを取得
        auto mvOpt = n_compoapi::GetMoveComponent(eid);
        if (!mvOpt) return;
        n_component::MoveComponent mc = *mvOpt;

        // ImGui 入力状態を参照
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) return;

        Vec2 dir{ 0.0f, 0.0f };

        // 方向Keyの受付
        if (mc.directionLeftKey != ImGuiKey_None && ImGui::IsKeyDown(mc.directionLeftKey)) dir[0] -= 1.0f;
        if (mc.directionRightKey != ImGuiKey_None && ImGui::IsKeyDown(mc.directionRightKey)) dir[0] += 1.0f;
        if (mc.directionUpKey != ImGuiKey_None && ImGui::IsKeyDown(mc.directionUpKey)) dir[1] -= 1.0f;
        if (mc.directionDownKey != ImGuiKey_None && ImGui::IsKeyDown(mc.directionDownKey)) dir[1] += 1.0f;

        // 正規化
        float len2 = dir[0] * dir[0] + dir[1] * dir[1];
        if (len2 > 1e-8f) 
        { 
            float inv = 1.0f / std::sqrt(len2);
            mc.direction[0] = dir[0] * inv;
            mc.direction[1] = dir[1] * inv;
        }
        else 
        { 
            mc.direction[0] = 0.0f;
            mc.direction[1] = 0.0f;
        }

        // Key押下フレームの検出
        // ジャンプ（押下フレーム）
        bool jumpPressed = (mc.jumpKey != ImGuiKey_None) && ImGui::IsKeyPressed(mc.jumpKey, false);
        if (jumpPressed)
        { 
            mc.jump = 1.0f; // 値の意味は MoveSystem 側で解釈（例：初速倍率) 
        }

        // 変化判定：前回の MoveComponent と差分があるときだけ送る
        const float EPS = 1e-4f; //仮の値　今後どう変わるかは挙動を見てゲームに適した値に変更する
        bool dirChanged = (std::fabs(mc.direction[0] - mvOpt->direction[0]) > EPS ||
                           std::fabs(mc.direction[1] - mvOpt->direction[1]) > EPS);
        bool jumpChanged = (jumpPressed && mvOpt->jump == 0.0f);

        // 変化なし
        if (!dirChanged && !jumpChanged) return;

        // 変化があればゲームスレッドへ反映
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid, mc]() {
            // ゲームスレッドで最新の MoveComponent を取得上書き
            if (auto curOpt = n_compoapi::GetMoveComponent(eid)) {
                auto cur = *curOpt;
                cur.direction[0] = mc.direction[0];
                cur.direction[1] = mc.direction[1];
                // jump は要求フラグなので上書き
                if (mc.jump > 0.0f) cur.jump = mc.jump;
                n_compoapi::SetMoveComponent(eid, cur);
            }
            else
            {
                // 万が一 MoveComponent が消えてたら付けなおす
                n_compoapi::SetMoveComponent(eid, mc);
            }
        });

    }


} // namespace Input