/********************************
 *�Q�[�����̃t���[�����Ԃ����߂�*
 ********************************/

#pragma once
#include <chrono>
#include <cstdint>

namespace frame {

    class Time {
    public:
        // �Q�[���J�n����1�񂾂��Ă�
        static void Start_Time();

        // ���t���[���ĂԁA�f���^�^�C����Ԃ�
        static float Update_Time();

        // �ŐV�̃f���^�^�C��
        static float GetDeltaTime();

        // �݌v�t���[����
        static uint64_t GetFrameCount();

    private:
        using Clock = std::chrono::high_resolution_clock;
        static Clock::time_point prevTime;
        static float              deltaTime;
        static uint64_t           frameCount;
    };

} // namespace time