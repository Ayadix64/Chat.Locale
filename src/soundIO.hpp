#ifndef SOUND_IO
#define SOUND_IO
#include <portaudio.h>
#include "utils.hpp"

#ifdef SOUND_IO_CPP
void MicrophoneInit();
void SpeakersInit();
#endif

extern std::function<void(float* r , unsigned int lng)> readSoundFromMicrophoneHandler;
extern std::vector<float> dataToPlay;
void soundIOInit();
void soundIOStop(); 


#endif
