#include "include/time.h"
#include "include/game.h"

namespace n_time {
    // 毎フレーム呼び出し
    double GetFrameTimeSeconds()
    {
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        duration<double> diff = now - lastTime;
        lastTime = now;
        double dt = diff.count(); // 秒
        // 極端に大きな dt を防ぐ（デバッグや一時停止復帰時のスパイク対策）
        const double maxDt = 0.25; // 250ms
        if (dt > maxDt) dt = maxDt;
        return dt;
    }

} // namespace time