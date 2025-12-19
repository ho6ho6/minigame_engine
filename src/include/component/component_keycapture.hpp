#pragma once
#include <cstdint>
#include <string>
#include <functional>


const char* GetKeyName(int key);

// StartKeyCaptureFor: entityId, purpose (任意文字列), callback(int key)
// callback は UI スレッドで呼ばれます
void StartKeyCaptureFor(int64_t entityId, const char* purpose, std::function<void(int)> callback);

// ImGuiKeyの定義
//int DetectImGuiKeyPressed();

// 毎フレーム UI 側で呼ぶ関数（ImGui のフレーム内）
void RenderKeyCaptureModal();