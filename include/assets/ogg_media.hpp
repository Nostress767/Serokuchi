#pragma once

#include <map>
#include <memory>
#include <string>

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

  std::string name;
  ma_sound sound;

public:
  OggMedia(std::string file, u8* bin, size_t sz);
  ~OggMedia();

  // TODO: add remaining necessary methods
  void load(AudioFlag flags = AudioFlag::defaulto, AudioGroup group = AudioGroup::globalMixer);
  void loadBGM();
  void unload();
  void start();
  void stop();
  void toggle();
  void seek(i32 frame);
  void play(AudioGroup group = AudioGroup::SFX);
  bool isLooping();
  bool isPlaying();
  static void setGroupVolume(f32 volume, AudioGroup group = AudioGroup::globalMixer);
};

#define ___include_ogg(filename, extension) \
  media_binary(filename, extension) \
  extern OggMedia filename

#define ___define_ogg(filename, extension) \
  OggMedia filename (#filename, media_start(filename, extension), media_size(filename, extension))

