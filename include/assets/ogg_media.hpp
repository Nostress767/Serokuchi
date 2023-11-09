#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "external/miniaudio.h"
#include "media.hpp"

enum class AudioFlag : u32 {
  defaulto = MA_SOUND_FLAG_ASYNC,
  decode = MA_SOUND_FLAG_DECODE
};

enum class AudioGroup : u32 {
  globalMixer, SFX, BGM
};

class OggMedia : private Media{
private:
  inline static std::unique_ptr<ma_engine> audioEngine = nullptr;
  inline static std::map<AudioGroup, ma_sound_group> audioGroups;
  inline static std::map<AudioGroup, i32> audioGroupVolumes;
  inline static i32 volumeStepAmount = 100;
  inline static std::vector<f32> volumeSteps;

  std::string name;
  ma_sound sound;

public:
  OggMedia(std::string file, u8* bin, size_t sz);

  bool isLooping();
/// NOTE: this only works for initialized sounds (i.e. not for ::play)
  bool isPlaying();
///
  void load(AudioFlag flags = AudioFlag::defaulto, AudioGroup group = AudioGroup::globalMixer);
  void loadBGM();
  void play(AudioGroup group = AudioGroup::SFX);
  void seek(i32 frame);
  void start();
  void stop();
  void toggle();
  void unload();

  static void setGroupVolume(f32 volume, AudioGroup group = AudioGroup::globalMixer);
};

#define ___include_ogg(filename, extension) \
  media_binary(filename, extension) \
  extern OggMedia filename

#define ___define_ogg(filename, extension) \
  OggMedia filename (#filename, media_start(filename, extension), media_size(filename, extension))

