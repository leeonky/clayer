#ifndef LSDL2_H
#define LSDL2_H

#include <functional>

extern "C" {
#include <SDL2/SDL.h>
}

int SDL_CreateWindow(const char *, int, int, int, int, Uint32, const std::function<int(SDL_Window *)> &);

int SDL_CreateTexture(SDL_Window *, int, int, Uint32, const std::function<int(int, int, SDL_Renderer *, SDL_Texture *)> &);

int SDL_PresentYUV(SDL_Renderer *, SDL_Texture *, uint8_t **, int *);

#endif