#ifndef LSDL2_H
#define LSDL2_H

#include <functional>

extern "C" {
#include <SDL2/SDL.h>
}

int SDL_CreateWindow(const char *, int, int, int, int, Uint32, const std::function<int(SDL_Window *)> &);

#endif
