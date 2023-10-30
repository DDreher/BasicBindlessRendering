#include "TickTimer.h"

TickTimer::TickTimer()
    : current_(std::chrono::high_resolution_clock::now())
    , prev_(std::chrono::high_resolution_clock::now())
{
}

void TickTimer::Update()
{
    current_ = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = current_ - prev_;
    elapsed_ = duration.count();

    prev_ = current_;
    accumulated_time_ += elapsed_;

    // Cap so we don't get weird artifacts when debugging
    accumulated_time_ = std::min(accumulated_time_, TICK_TIME * MAX_TICKS_PER_FRAME);
}
