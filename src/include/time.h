/********************************
 *ゲーム内のフレーム時間を決める*
 ********************************/

#ifndef TIME_H
#define TIME_H

#pragma once
#include <chrono>
#include <cstdint>
static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
namespace n_time
{
    // 毎フレーム呼ぶ、デルタタイムを返す
    double GetFrameTimeSeconds();

} // namespace time

#endif // TIME_HPP