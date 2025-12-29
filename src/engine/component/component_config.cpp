#include "include/component/component_config.hpp"
#include "include/component/component_keycapture.h"
#include "include/component/game_component.h"
#include "include/component/component_api.h"   // Has/Get/Set/Add/Remove ラッパー
#include "include/window_editor/window_scene.h"
#include <imgui.h>
#include <Windows.h>

// UI　コンポーネントの見た目はここで変わる
void RenderTransformInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetTransformComponent(entityId);
    if (!opt) {
        ImGui::Text("The Transform component is not attached");
        if (ImGui::Button("Add Transform")) {
            n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId]() {
                n_compoapi::AddTransformComponent(entityId);
                });
        }
        return;
    }

    auto t = *opt;
    bool changed = false;
    float pos[2]     = { t.position[0], t.position[1] };
    float rote[2]    = { t.rotation[0], t.rotation[1] };
    float scale[2]   = { t.scale[0], t.scale[1] };

    if (ImGui::DragFloat2("Transform", pos, 1.0f)) {
        t.position[0] = pos[0];
        t.position[1] = pos[1];
        changed = true;
    }
    if (ImGui::DragFloat2("Rotation", rote, 1.0f)) {
        t.rotation[0] = rote[0];
        t.rotation[1] = rote[1];
        changed = true;
    }
    if (ImGui::DragFloat2("Scale", scale, 1.0f)) {
        t.scale[0] = scale[0];
        t.scale[1] = scale[1];
        changed = true;
    }

    if (changed)
    {
        // ゲームスレッドへ投げる（UI スレッドから直接書き込まない）

        //fprintf(stderr, "[UI] Enqueue before entity=%lld thread=%zu\n",
        //    (long long)entityId, (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
        //fflush(stderr);

        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId, t]() {
            n_compoapi::SetTransformComponent(entityId, t);
            });
    }

    if (ImGui::Button("Remove Transform")) {
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId]() {
            n_compoapi::RemoveTransformComponent(entityId);
            });
    }
}

void RenderMoveInspector(int64_t entityId)
{   
    auto opt = n_compoapi::GetMoveComponent(entityId);
    if (!opt) {
        ImGui::Text("The Move component is not attached");
        if (ImGui::Button("Add Move")) {
            n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId]() {
                n_compoapi::AddMoveComponent(entityId);
                });
        }
        return;
    }

    auto mc = *opt;
    bool changed = false;
    float dir[2] = { mc.direction[0], mc.direction[1] };

    if (ImGui::DragFloat("Speed", &mc.speed, 100.0f)) changed = true;
    //if (ImGui::DragFloat2("Direction", dir, 1.0f)) {
    //    mc.direction[0] = dir[0];
    //    mc.direction[1] = dir[1];
    //    changed = true;
    //}
    if (ImGui::DragFloat("Acceleration", &mc.acceleration, 100.0f)) changed = true;
    if (ImGui::DragFloat("Jump Strength", &mc.jump, 100.0f)) changed = true;


    if (changed) {
        // コンポーネントストレージに書き戻す（ゲームスレッド経由）
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId, mc]() {
            n_compoapi::SetMoveComponent(entityId, mc);
            });

        // **注意**: ここで直接 Transform を Set すると初期値が連続で更新されて動かせなくなる
    }

    /* Keyの変更は即時反映 */
    // 起こることは無いと思うが、即時反映を行うことでKey割り当ての競合を防ぐ


    // JumpKey
    ShowKeyBind("Jump Key",
        [&mc]() -> ImGuiKey {return mc.jumpKey; },
        [entityId](ImGuiKey imKey) {
                if (auto opt2 = n_compoapi::GetMoveComponent(entityId))
                {
                    auto m2 = *opt2;
                    m2.jumpKey = imKey;
                    n_compoapi::SetMoveComponent(entityId, m2);
                }
            },
        entityId);

    // UpKey
    ShowKeyBind("UP Key",
        [&mc]() -> ImGuiKey {return mc.directionUpKey; },
        [entityId](ImGuiKey imKey) {
            if (auto opt2 = n_compoapi::GetMoveComponent(entityId))
            {
                auto m2 = *opt2;
                m2.directionUpKey = imKey;
                n_compoapi::SetMoveComponent(entityId, m2);
            }
        },
    entityId);

    // RightKey
    ShowKeyBind("Right Key",
        [&mc]() -> ImGuiKey {return mc.directionRightKey; },
        [entityId](ImGuiKey imKey) {
            if (auto opt2 = n_compoapi::GetMoveComponent(entityId))
            {
                auto m2 = *opt2;
                m2.directionRightKey = imKey;
                n_compoapi::SetMoveComponent(entityId, m2);
            }
        },
        entityId);

    // Down Key
    ShowKeyBind("Down Key",
        [&mc]() -> ImGuiKey {return mc.directionDownKey; },
        [entityId](ImGuiKey imKey) {
            if (auto opt2 = n_compoapi::GetMoveComponent(entityId))
            {
                auto m2 = *opt2;
                m2.directionDownKey = imKey;
                n_compoapi::SetMoveComponent(entityId, m2);
            }
        },
        entityId);

    //Left Key
    ShowKeyBind("Left Key",
        [&mc]() -> ImGuiKey {return mc.directionLeftKey; },
        [entityId](ImGuiKey imKey) {
            if (auto opt2 = n_compoapi::GetMoveComponent(entityId))
            {
                auto m2 = *opt2;
                m2.directionLeftKey = imKey;
                n_compoapi::SetMoveComponent(entityId, m2);
            }
        },
        entityId);

    if (ImGui::Button("Remove Move")) {
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([entityId]() {
            n_compoapi::RemoveMoveComponent(entityId);
            });
    }
}

void RenderLightInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetLightComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Light component is not attached");
        if (ImGui::Button("Add Light")) n_compoapi::AddLightComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;
    float color[3] = { mc.color[0], mc.color[1], mc.color[2] };

    if (ImGui::DragFloat("Light_Range", &mc.range, 0.1f)) changed = true;
    if (ImGui::DragFloat3("Light_Color", color, 0.1f))
    {
        mc.color[0] = color[0];
        mc.color[1] = color[1];
        mc.color[2] = color[2];
        changed = true;
    }
    if (ImGui::DragFloat("Light_Intensity", &mc.intensity, 0.1f)) changed = true;

    if (changed) {
        n_compoapi::SetLightComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Light")) {
        n_compoapi::RemoveLightComponent(entityId);
    }
}

void RenderRigidbodyInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetRigidbodyComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Rigidbody component is not attached");
        if (ImGui::Button("Add Rigidbody")) n_compoapi::AddRigidbodyComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;

    float Gravity[2] = {mc.gravity[0], mc.gravity[1]};

    if (ImGui::DragFloat2("Gravity", Gravity, 100.0f))
    {
        mc.gravity[0] = Gravity[0];
        mc.gravity[1] = Gravity[1];
        changed = true;
    }
    //if (ImGui::DragFloat("JumpElapsed", &mc.jumpElapsed, 1.0f)) changed = true;
    if (ImGui::Checkbox("IsGround", &mc.isGround)) changed = true;
    if (ImGui::Checkbox("IsJump", &mc.isJump)) changed = true;

    if (changed) {
        n_compoapi::SetRigidbodyComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Rigidbody")) {
        n_compoapi::RemoveRigidbodyComponent(entityId);
    }
}

void RenderStartInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetStartComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Start component is not attached");
        if (ImGui::Button("Add Start")) n_compoapi::AddStartComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;

    if (ImGui::DragFloat("Start_Range", &mc.spawnRadius, 0.1f)) changed = true;
    if (ImGui::DragInt("Start_SpawnPriority", &mc.priority, 1)) changed = true;
    if (ImGui::Checkbox("Start_IsActive", &mc.active)) changed = true;

    if (changed)
    {
        n_compoapi::SetStartComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Start")) {
        n_compoapi::RemoveStartComponent(entityId);
    }
}

void RenderFinishInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetFinishComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Finish component is not attached");
        if (ImGui::Button("Add Finish")) n_compoapi::AddFinishComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;

    if (ImGui::DragFloat("Finish_Range", &mc.radius, 0.1f)) changed = true;
    if (ImGui::Checkbox("Finish_IsTrigger", &mc.oneShot)) changed = true;
    if (ImGui::Checkbox("Finish_IsActive", &mc.active)) changed = true;

    if (changed)
    {
        n_compoapi::SetFinishComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Finish")) {
        n_compoapi::RemoveFinishComponent(entityId);
    }
}

void RenderIsPlayerInspector(int64_t entityId)
{
    auto opt = n_compoapi::GetIsPlayerComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The IsPlayer component is not attached");
        if (ImGui::Button("Add IsPlayer")) n_compoapi::AddIsPlayerComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;

    if (ImGui::DragScalar("PlaterID", ImGuiDataType_S64, &mc.PlayerId, 1.0f)) changed = true;
    if (ImGui::DragScalar("EntityID", ImGuiDataType_S64, &mc.EntityId, 1.0f)) changed = true;
    if (ImGui::Checkbox("IsPlayerActive", &mc.IsPlayerActive)) changed = true;
    if (ImGui::Checkbox("IsEnemyActive", &mc.IsEnemyActive)) changed = true;

    if (changed)
    {
        n_compoapi::SetIsPlayerComponent(entityId, mc);
    }

    if (ImGui::Button("Remove IsPlayer")) {
        n_compoapi::RemoveIsPlayerComponent(entityId);
    }
}

const char* GetImGuiKeyNameSafe(ImGuiKey imKey)
{
    if (imKey == ImGuiKey_None) return "None";
    auto it = g_ImGuiKeyNames.find(imKey);
    if (it != g_ImGuiKeyNames.end()) return it->second;
    static char buf[32];
    snprintf(buf, sizeof(buf), "Key(%d)", (int)imKey); return buf;
}

void ShowKeyBind(const char* label, std::function<ImGuiKey()> getCurrent, std::function<void(ImGuiKey)> setOnGameThread, int64_t entityId)
{
    ImGuiKey curKey = getCurrent();
    const char* name = GetImGuiKeyNameSafe(curKey);
    ImGui::Text("%s: %s", label, name);
    ImGui::SameLine();

    if (ImGui::Button((std::string(label) + "change").c_str()))
    {
        // StartKeyCaptureForは ImGuiKey を返すCallbackを受け取る
        StartKeyCaptureFor(entityId, label, [setOnGameThread](ImGuiKey imKey) {
            // UI スレッド -> ゲームスレッドへ
            n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([setOnGameThread, imKey]() {
                setOnGameThread(imKey);
                });
            });
    }

    ImGui::SameLine();
    if (ImGui::Button((std::string(label) + "Clear").c_str())) {
        n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([setOnGameThread]() {
            setOnGameThread(ImGuiKey_None);
            });
    }
}