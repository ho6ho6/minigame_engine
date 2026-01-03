#ifndef COMPO_KEYCAP_H
#define COMPO_KEYCAP_H

#include <cstdint>
#include <string>
#include <functional>
#include "imgui.h"

const char* GetKeyName(int key);

// StartKeyCaptureFor: entityId, purpose (任意文字列), callback(int key)
// callback は UI スレッドで呼ばれます
void StartKeyCaptureFor(int64_t entityId, const char* purpose, std::function<void(ImGuiKey)> callback);

// ImGuiKeyの定義
//int DetectImGuiKeyPressed();

// 毎フレーム UI 側で呼ぶ関数（ImGui のフレーム内）
void RenderKeyCaptureModal();

// キャプチャ状態
struct KeyCaptureState
{
    bool open = false;
    int64_t entityId = -1;
    std::string purpose;
    std::function<void(ImGuiKey)> callback;
    // オプション: タイムアウトやキャンセル用フラグ
};

#endif // !COMPO_KEYCAP_H