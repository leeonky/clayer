#ifndef MEDIA_H
#define MEDIA_H

#include <functional>
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"

Uint32 AVPixelFormat_to_SDL(const char *);

int open_video(iobus &, const char *, int, int, int, int, Uint32, const std::function<int(SDL_Window *, SDL_Renderer *, int, int, SDL_Texture *)> &);

#endif
