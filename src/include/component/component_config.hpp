#pragma once
#include <cstdint>
#include <unordered_map>
#include <functional>
#include "imgui.h"

// インスペクターウィンドウに表示するコンポーネント
void RenderTransformInspector(int64_t entityId);
void RenderMoveInspector(int64_t entityId);
void RenderIsPlayerInspector(int64_t entityId);
void RenderRigidbodyInspector(int64_t entityId);
void RenderStartInspector(int64_t entityId);
void RenderFinishInspector(int64_t entityId);
void RenderLightInspector(int64_t entityId);

const char* GetImGuiKeyNameSafe(ImGuiKey imKey);

// GetImGuiKeyNameSafeのヘルパ
void ShowKeyBind(const char* label, std::function<ImGuiKey()> getCurrent, std::function<void(ImGuiKey)> setOnGameThread, int64_t entityId);

static const std::unordered_map<ImGuiKey, const char*> g_ImGuiKeyNames = {
    {ImGuiKey_A, "A"},
    {ImGuiKey_B, "B"},
    {ImGuiKey_C, "C"},
    {ImGuiKey_D, "D"},
    {ImGuiKey_E, "E"},
    {ImGuiKey_F, "F"},
    {ImGuiKey_G, "G"},
    {ImGuiKey_H, "H"},
    {ImGuiKey_I, "I"},
    {ImGuiKey_J, "J"},
    {ImGuiKey_K, "K"},
    {ImGuiKey_L, "L"},
    {ImGuiKey_M, "M"},
    {ImGuiKey_N, "N"},
    {ImGuiKey_O, "O"},
    {ImGuiKey_P, "P"},
    {ImGuiKey_Q, "Q"},
    {ImGuiKey_R, "R"},
    {ImGuiKey_S, "S"},
    {ImGuiKey_T, "T"},
    {ImGuiKey_U, "U"},
    {ImGuiKey_V, "V"},
    {ImGuiKey_W, "W"},
    {ImGuiKey_X, "X"},
    {ImGuiKey_Y, "Y"},
    {ImGuiKey_Z, "Z"},
    {ImGuiKey_Space, "Space"},
    {ImGuiKey_Tab, "Tab"},
    {ImGuiKey_LeftShift, "LeftShift"},
    {ImGuiKey_LeftCtrl, "LeftControl"},
    {ImGuiKey_Escape, "Escape"},
    {ImGuiKey_RightShift, "RightShift"},
    {ImGuiKey_RightCtrl, "RightCtrl"},
    {ImGuiKey_Enter, "Enter"},
    {ImGuiKey_UpArrow, "UpArrow"},
    {ImGuiKey_RightArrow, "RightArrow"},
    {ImGuiKey_DownArrow, "DownArrow"},
    {ImGuiKey_LeftArrow, "LeftArrow"},
};