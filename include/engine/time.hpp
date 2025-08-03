/********************************
 *ゲーム内のフレーム時間を決める*
 ********************************/

#pragma once
#include <chrono>
#include <cstdint>

namespace frame {

    class Time {
    public:
        // ゲーム開始時に1回だけ呼ぶ
        static void Start_Time();

        // 毎フレーム呼ぶ、デルタタイムを返す
        static float Update_Time();

        // 最新のデルタタイム
        static float GetDeltaTime();

        // 累計フレーム数
        static uint64_t GetFrameCount();

    private:
        using Clock = std::chrono::high_resolution_clock;
        static Clock::time_point prevTime;
        static float              deltaTime;
        static uint64_t           frameCount;
    };

} // namespace time