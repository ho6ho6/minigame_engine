/**/

#include "include/time.hpp"

namespace n_time {

    // static �����o�̃C���X�^���X���i�K�{�j
    Time::Clock::time_point Time::prevTime;
    float              Time::deltaTime = 0.0f;
    uint64_t           Time::frameCount = 0;

    // �Q�[���J�n���ɌĂяo��
    void Time::Start_Time() {
        prevTime = Clock::now();
        frameCount = 0;
    }

    // ���t���[���Ăяo��
    float Time::Update_Time() {
        auto now = Clock::now();
        deltaTime = std::chrono::duration<float>(now - prevTime).count();
        prevTime = now;
        ++frameCount;
        return deltaTime;
    }

    // �ŐV�f���^�^�C���擾
    float Time::GetDeltaTime() {
        return deltaTime;
    }

    // �t���[�����擾
    uint64_t Time::GetFrameCount() {
        return frameCount;
    }

} // namespace time