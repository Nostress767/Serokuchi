#pragma once

#include <chrono>

#include "types.hpp"

namespace TimeUnit{
  using nanoseconds  = std::chrono::duration<double, std::ratio<1, 1000000000>>;
  using microseconds = std::chrono::duration<double, std::ratio<1, 1000000>>;
  using milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;
  using centiseconds = std::chrono::duration<double, std::ratio<1, 100>>;
  using deciseconds  = std::chrono::duration<double, std::ratio<1, 10>>;
  using seconds      = std::chrono::duration<double, std::ratio<1, 1>>;
};

class Clock {
  private:
  std::chrono::time_point<std::chrono::system_clock> start, past, now;
  u64 totalFrames = 0;
  u16 frame = 0, framesLastSecond = 0;

  public:
  Clock();

  void reset();

  template<typename Time>
  double getTotalElapsed();

  template<typename Time>
  double getLastFrameElapsed();

  template<typename Time>
  double getLastSecondElapsed();

  u64 getFrame();
  u16 getTotalFrames();
  u16 getFPS();
  double get();

  Clock& tick();
};

#ifdef DEBUG

extern Clock ___globalLogClock;

#endif
