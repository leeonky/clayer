#ifndef MEDIA_H
#define MEDIA_H

#include <functional>
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"

Uint32 AVPixelFormat_to_SDL(enum AVPixelFormat);
int video_event(iobus &, const std::function<int(int, int, enum AVPixelFormat)> &);
int buffer_event(iobus &, const std::function<int(int, size_t, int, int)> &);
#endif
