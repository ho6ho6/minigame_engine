#include "include/component/component_config.hpp"
#include "include/component/component_keycapture.hpp"
#include "include/component/component_api.hpp"   // Has/Get/Set/Add/Remove ラッパー
#include <imgui.h>
#include <Windows.h>

void RenderTransformInspector(int64_t entityId)
{
    // GetMoveComponent の結果で分岐
    auto opt = n_compoapi::GetTransformComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Transform component is not attached");
        if (ImGui::Button("Add Transform")) n_compoapi::AddTransformComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;
    float pos[2] = {mc.position[0], mc.position[1]};
    float rote[2] = {mc.rotation[0], mc.rotation[1]};
    float scale[2] = {mc.scale[0] , mc.scale[1]};

    if (ImGui::DragFloat2("Transform", pos, 1.0f))
    {
        mc.position[0] = pos[0];
        mc.position[1] = pos[1];
        changed = true;
    }
    if (ImGui::DragFloat2("Rotation", rote, 1.0f)) {
        mc.rotation[0] = rote[0];
        mc.rotation[1] = rote[1];
        changed = true;
    }
    if (ImGui::DragFloat2("Scale", scale, 1.0f))
    {
        mc.scale[0] = scale[0];
        mc.scale[1] = scale[1];
        changed = true;
    }
    if (changed) {
        n_compoapi::SetTransformComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Transform")) {
        n_compoapi::RemoveTransformComponent(entityId);
    }
}

void RenderMoveInspector(int64_t entityId)
{   
    // GetMoveComponent の結果で分岐
    auto opt = n_compoapi::GetMoveComponent(entityId);
    if (!opt)
    {
        ImGui::Text("The Move component is not attached");
        if (ImGui::Button("Add Move")) n_compoapi::AddMoveComponent(entityId);
        return;
    }

    auto mc = *opt;
    bool changed = false;
    float dir[2] = { mc.direction[0], mc.direction[1]};

    if (ImGui::DragFloat("Speed", &mc.speed, 0.1f)) changed = true;
    if (ImGui::DragFloat2("Direction", dir, 0.1f))
    {
        mc.direction[0] = dir[0];
        mc.direction[1] = dir[1];
        changed = true;
    }
    if (ImGui::DragFloat("Acceleration", &mc.acceleration, 0.1f)) changed = true;
    if (ImGui::DragFloat("Jump Strength", &mc.jump, 0.1f)) changed = true;

    // mc.jumpKey は VK コードを保持する想定（未設定は -1）
    const char* keyName = (mc.jumpKey >= 0) ? GetKeyName(mc.jumpKey) : "None";
    ImGui::Text("Jump Key: %s", keyName);
    ImGui::SameLine();

    if (ImGui::Button("Capture Key")) {
        // StartKeyCaptureFor は Win32 実装で VK をコールバックに渡す想定
        StartKeyCaptureFor(entityId, "jump", [entityId](int vk) {
            auto opt2 = n_compoapi::GetMoveComponent(entityId);
            if (opt2) {
                auto m2 = *opt2;
                m2.jumpKey = vk; // VK をそのまま保存
                n_compoapi::SetMoveComponent(entityId, m2);
            }
            });
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        mc.jumpKey = -1;
        changed = true;
    }

    if (changed) {
        n_compoapi::SetMoveComponent(entityId, mc);
    }

    if (ImGui::Button("Remove Move")) {
        n_compoapi::RemoveMoveComponent(entityId);
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

    if (ImGui::DragFloat("Gravity", &mc.gravity, 0.1f)) changed = true;
    if (ImGui::Checkbox("IsGround", &mc.isGround)) changed = true;

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
    if (ImGui::Checkbox("Start_sActive", &mc.active)) changed = true;

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