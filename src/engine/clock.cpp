#include "engine/clock.hpp"

Clock::Clock(){
  past = now = start = std::chrono::system_clock::now();
}

void Clock::reset(){
  past = now = start = std::chrono::system_clock::now();
  totalFrames = frame = framesLastSecond = 0;
}

template<typename Time>
double Clock::getTotalElapsed(){
  Time delta = now - start;
  return delta.count();
}

/// TODO: implement the rest as needed
template double Clock::getTotalElapsed<TimeUnit::seconds>();
template double Clock::getLastFrameElapsed<TimeUnit::seconds>();
///

template<typename Time>
double Clock::getLastFrameElapsed(){
  Time delta = now - past;
  return delta.count();
}

template<typename Time>
double Clock::getLastSecondElapsed(){
  Time delta = (now - start) - std::chrono::floor<std::chrono::seconds>(now - start);
  return delta.count();
}

u64 Clock::getFrame(){ return frame; }
u64 Clock::getTotalFrames(){ return totalFrames; }
u64 Clock::getFPS(){ return framesLastSecond; }
double Clock::get(){ return getLastFrameElapsed<TimeUnit::seconds>(); }

Clock& Clock::tick(){
  frame++;
  totalFrames++;
  past = now;
  now = std::chrono::system_clock::now();

  if(getLastSecondElapsed<TimeUnit::seconds>() - get() <= 0){
    framesLastSecond = frame;
    frame = 0;
  }

  return *this;
}

#ifdef DEBUG
Clock ___globalLogClock;
#endif
