#pragma once
#include <chrono>

class TickTimer
{
public:
    TickTimer();

    void Update();

    static inline constexpr uint32_t MAX_TICKS_PER_FRAME = 144;
    static inline constexpr double TICK_TIME = 1.0 / 144.0;  // in seconds

    std::chrono::high_resolution_clock::time_point current_;
    std::chrono::high_resolution_clock::time_point prev_;

    double accumulated_time_ = 0.0;
    double elapsed_ = 0.0;
};
