#include "include/component/component_keycapture.h"
#include "include/component/component_config.hpp"
#include <imgui.h>
#include <unordered_map>
#include <mutex>
#include <Windows.h>


const char* GetKeyName(int key)
{
    // まずよく使う VK をハードコードで処理
    switch (key) {
    case VK_SPACE:   return "Space";
    case VK_LEFT:    return "Left";
    case VK_UP:      return "Up";
    case VK_RIGHT:   return "Right";
    case VK_DOWN:    return "Down";
    case VK_ESCAPE:  return "Esc";
    case VK_RETURN:  return "Enter";
    case VK_TAB:     return "Tab";
    case VK_SHIFT:   return "Shift";
    case VK_CONTROL: return "Ctrl";
    case VK_MENU:    return "Alt";
        // 必要なら VK_F1..VK_F12, VK_NUMPAD0.. 等を追加
    }

    // 英大文字 A..Z はそのまま表示
    if (key >= 'A' && key <= 'Z') {
        thread_local char buf[2];
        buf[0] = static_cast<char>(key);
        buf[1] = '\0';
        return buf;
    }

    // 数字 0..9（ASCII）
    if (key >= '0' && key <= '9') {
        thread_local char buf[2];
        buf[0] = static_cast<char>(key);
        buf[1] = '\0';
        return buf;
    }

    // それ以外はマップで補完（必要に応じて拡張）
    static const std::unordered_map<int, const char*> extra = {
        { VK_OEM_1, ";" }, { VK_OEM_PLUS, "+" }, { VK_OEM_COMMA, "," },
        { VK_OEM_MINUS, "-" }, { VK_OEM_PERIOD, "." }, { VK_OEM_2, "/" },
        // 追加: VK_OEM_3, VK_OEM_4..VK_OEM_8 など
    };
    auto it = extra.find(key);
    if (it != extra.end()) return it->second;

    return "Unknown";
}



static ImGuiKey DetectImGuiKeyPressed() {
    ImGuiIO& io = ImGui::GetIO(); // ImGuiKey_NamedKey_BEGIN 〜 ImGuiKey_COUNT の範囲を走査 
    for (int k = (int)ImGuiKey_NamedKey_BEGIN; k < (int)ImGuiKey_COUNT; ++k) 
    { 
        ImGuiKey key = (ImGuiKey)k; 
        if (ImGui::IsKeyDown(key)) return key;
    } 
    return ImGuiKey_None;
}

static KeyCaptureState g_keyCapture;
static std::mutex g_keyCaptureMutex;

// Start capture: 設定してモーダルを開く
void StartKeyCaptureFor(int64_t entityId, const char* purpose, std::function<void(ImGuiKey)> callback)
{
    {
        std::lock_guard<std::mutex> lk(g_keyCaptureMutex);
        g_keyCapture.open = true;
        g_keyCapture.entityId = entityId;
        g_keyCapture.purpose = purpose ? purpose : "";
        g_keyCapture.callback = std::move(callback);
    } // ここでミューテックスは解放

    // ImGui 呼び出しはロック外で行う
    ImGui::OpenPopup("Key Capture");
}

// UI スレッドで毎フレーム呼ぶ。キー入力検出とコールバック呼び出しを行う。
void RenderKeyCaptureModal()
{
    {
        std::lock_guard<std::mutex> lk(g_keyCaptureMutex);
        if (!g_keyCapture.open) return;
    }

    if (ImGui::BeginPopupModal("Key Capture", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press any key to bind (%s)", g_keyCapture.purpose.c_str());
        ImGui::Separator();
        ImGui::Text("Press Esc to cancel.");

        ImGuiKey detected = DetectImGuiKeyPressed(); // または ImGui 検出版

        const char* cur = (detected != ImGuiKey_None) ? GetImGuiKeyNameSafe(detected) : "None";
        ImGui::Text("Current: %s (0x%02X)", cur, detected);

        if (detected != 0)
        {
            std::function<void(ImGuiKey)> cb;
            {
                std::lock_guard<std::mutex> lk(g_keyCaptureMutex);
                cb = std::move(g_keyCapture.callback); // コールバックを取り出す
                g_keyCapture.open = false;
                g_keyCapture.entityId = -1;
                g_keyCapture.purpose.clear();
            } // ミューテックス解放

            ImGui::CloseCurrentPopup(); // ImGui 呼び出しはロック外
            if (cb) cb(detected);       // コールバックはロック外で呼ぶ
        }

        if (ImGui::Button("Cancel")) {
            std::function<void(ImGuiKey)> cb;
            {
                std::lock_guard<std::mutex> lk(g_keyCaptureMutex);
                g_keyCapture.open = false;
                cb = std::move(g_keyCapture.callback);
                g_keyCapture.entityId = -1;
                g_keyCapture.purpose.clear();
            }
            ImGui::CloseCurrentPopup();
            // 通常キャンセル時はコールバックを呼ばないが、必要ならここで呼ぶ（ロック外）
        }

        ImGui::EndPopup();
    }
    else {
        ImGui::OpenPopup("Key Capture");
    }
}