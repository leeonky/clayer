#ifndef LPORT_AUDIO_LPORT_AUDIO_H
#define LPORT_AUDIO_LPORT_AUDIO_H

#include <cunitexd.h>
#include <portaudio.h>
#include <functional>

int Pa_Init_OpenOutputStream(PaDeviceIndex, int, int, PaSampleFormat, const std::function<int(PaStream *)> &);

int64_t Pa_GetStreamLast(PaStream *, long, int);

#endif

