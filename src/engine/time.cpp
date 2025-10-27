/**/

#include "include/time.hpp"

namespace n_time {

    // static メンバのインスタンス化（必須）
    Time::Clock::time_point Time::prevTime;
    float              Time::deltaTime = 0.0f;
    uint64_t           Time::frameCount = 0;

    // ゲーム開始時に呼び出し
    void Time::Start_Time() {
        prevTime = Clock::now();
        frameCount = 0;
    }

    // 毎フレーム呼び出し
    float Time::Update_Time() {
        auto now = Clock::now();
        deltaTime = std::chrono::duration<float>(now - prevTime).count();
        prevTime = now;
        ++frameCount;
        return deltaTime;
    }

    // 最新デルタタイム取得
    float Time::GetDeltaTime() {
        return deltaTime;
    }

    // フレーム数取得
    uint64_t Time::GetFrameCount() {
        return frameCount;
    }

} // namespace time