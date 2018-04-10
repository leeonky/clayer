#ifndef LALSA_LALSA_H
#define LALSA_LALSA_H

#include <cunitexd.h>
#include <alsa/asoundlib.h>
#include <functional>

int snd_pcm_open(const char *, int, int, snd_pcm_format_t, const std::function<int(snd_pcm_t *)> &);

int snd_pcm_mmap_writei(snd_pcm_t *, const void *, int);

int64_t snd_pcm_delay(snd_pcm_t *, int);

#endif


