#include "assets/ogg_media.hpp"

#include "engine/clock.hpp"

// TODO: log everything

OggMedia::OggMedia(std::string file, u8* bin, size_t sz) : Media(bin, sz){
  if(!audioEngine){
    audioEngine = std::make_unique<ma_engine>();
    ma_engine_init(NULL, audioEngine.get());

    ma_sound_group globalMixer = {}, SFX = {}, BGM = {};
    audioGroups[AudioGroup::globalMixer] = globalMixer;
    audioGroups[AudioGroup::SFX] = SFX;
    audioGroups[AudioGroup::BGM] = BGM;
    ma_sound_group_init(audioEngine.get(), 0, NULL, &audioGroups[AudioGroup::globalMixer]);
    ma_sound_group_init(audioEngine.get(), 0, &audioGroups[AudioGroup::globalMixer], &audioGroups[AudioGroup::SFX]);
    ma_sound_group_init(audioEngine.get(), 0, &audioGroups[AudioGroup::globalMixer], &audioGroups[AudioGroup::BGM]);

    // NOTE: This isn't necessary, but left as a reminder
    //ma_sound_group_uninit(&audioGroups[AudioGroup::globalMixer]);
    //ma_sound_group_uninit(&audioGroups[AudioGroup::SFX]);
    //ma_sound_group_uninit(&audioGroups[AudioGroup::BGM]);

    for(i32 i = 0; i <= volumeStepAmount; i++){
      volumeSteps.push_back(static_cast<f32>(i)/volumeStepAmount);
    }
    audioGroupVolumes[AudioGroup::globalMixer] = volumeStepAmount;
    audioGroupVolumes[AudioGroup::SFX] = volumeStepAmount;
    audioGroupVolumes[AudioGroup::BGM] = volumeStepAmount;
  }
  name = file;
  ma_result r = ma_resource_manager_register_encoded_data(ma_engine_get_resource_manager(audioEngine.get()), name.c_str(), bin, sz);
  log_debug(name.c_str() << " (" << ma_result_description(r) << ")");
}

bool OggMedia::isLooping(){ return ma_sound_is_looping(&sound); }

/// NOTE: this only works for initialized sounds (i.e. not for ::play)
bool OggMedia::isPlaying(){ return ma_sound_is_playing(&sound); }
///

void OggMedia::load(AudioFlag flags, AudioGroup group){
  ma_sound_init_from_file(audioEngine.get(), name.c_str(), static_cast<u32>(flags), &audioGroups[group], NULL, &sound);
}

void OggMedia::loadBGM(){
  load(AudioFlag::decode, AudioGroup::BGM);
  ma_sound_set_looping(&sound, MA_TRUE);
}

void OggMedia::play(AudioGroup group){
  ma_result r = ma_engine_play_sound(audioEngine.get(), name.c_str(), &audioGroups[group]);
  log_debug(name.c_str() << " (" << ma_result_description(r) << ")");
}

void OggMedia::seek(i32 frame){
  ma_sound_seek_to_pcm_frame(&sound, frame);
}

void OggMedia::start(){ ma_sound_start(&sound); }

void OggMedia::stop(){ ma_sound_stop(&sound); }

void OggMedia::toggle(){
  isPlaying() ? stop() : start();
}

void OggMedia::unload(){ ma_sound_uninit(&sound); }

void OggMedia::setGroupVolume(f32 volume, AudioGroup group){
  ma_sound_group_set_volume(&audioGroups[group], volume);
}

