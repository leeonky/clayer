#ifndef LSDL2_H
#define LSDL2_H

#include <functional>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
}

int SDL_CreateWindow(const char *, int, int, int, int, Uint32, const std::function<int(SDL_Window *)> &);

int SDL_CreateTexture(SDL_Window *, int, int, Uint32, const std::function<int(int, int, SDL_Renderer *, SDL_Texture *)> &);

int SDL_UpdateAndCopyYUV(SDL_Renderer *, SDL_Texture *, uint8_t **, int *);

int SDL_PresentYUV(SDL_Renderer *, SDL_Texture *, uint8_t **, int *);

int SDL_OpenAudio(int, int, int, SDL_AudioFormat, const std::function<int(SDL_AudioDeviceID, const SDL_AudioSpec &)> &);

int SDL_QueueAudio(SDL_AudioDeviceID, const void *, int, SDL_AudioFormat, int);

int SDL_AudioLast(SDL_AudioDeviceID, const SDL_AudioSpec &);

int TTF_OpenFont(const char *, int, const std::function<int(TTF_Font *)> &);

int TTF_RenderUTF8_Blended(TTF_Font *, const char *, SDL_Color, const std::function<int(SDL_Surface *)> &);
#endif
