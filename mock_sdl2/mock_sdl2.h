#ifndef MOCK_SDL_
#define MOCK_SDL_

#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <SDL2/SDL.h>

extern_mock_function_1(int, SDL_InitSubSystem, Uint32);
extern_mock_void_function_1(SDL_QuitSubSystem, Uint32);

extern_mock_function_3(SDL_Renderer *, SDL_CreateRenderer, SDL_Window *, int, Uint32);
extern_mock_void_function_1(SDL_DestroyRenderer, SDL_Renderer *);

extern_mock_function_6(SDL_Window *, SDL_CreateWindow, const char *, int, int, int, int, Uint32);
extern_mock_void_function_1(SDL_DestroyWindow, SDL_Window *);

extern_mock_function_1(int, SDL_ShowCursor, int);

extern_mock_function_5(SDL_Texture *, SDL_CreateTexture, SDL_Renderer *, Uint32, int, int, int);
extern_mock_void_function_1(SDL_DestroyTexture, SDL_Texture *);

extern_mock_void_function_3(SDL_GL_GetDrawableSize, SDL_Window *, int *, int *);

extern_mock_function_8(int, SDL_UpdateYUVTexture, SDL_Texture *, const SDL_Rect *, const Uint8 *, int , const Uint8 *, int , const Uint8 *, int);
extern_mock_function_4(int, SDL_RenderCopy, SDL_Renderer *, SDL_Texture *, const SDL_Rect *, const SDL_Rect *);
extern_mock_void_function_1(SDL_RenderPresent, SDL_Renderer *);

extern_mock_function_5(SDL_AudioDeviceID, SDL_OpenAudioDevice, const char *, int, const SDL_AudioSpec *, SDL_AudioSpec *, int);
extern_mock_void_function_1(SDL_CloseAudioDevice, SDL_AudioDeviceID);
extern_mock_function_2(const char *, SDL_GetAudioDeviceName, int, int);
extern_mock_void_function_2(SDL_PauseAudioDevice, SDL_AudioDeviceID, int);
extern_mock_function_3(int, SDL_QueueAudio, SDL_AudioDeviceID, const void *, Uint32);

extern_mock_function_1(Uint32, SDL_GetQueuedAudioSize, SDL_AudioDeviceID);

extern_mock_function_3(int, SDL_GetRendererOutputSize, SDL_Renderer *, int *, int *);
extern_mock_function_5(int, SDL_QueryTexture, SDL_Texture *, Uint32 *, int *, int *, int *);

#ifdef __cplusplus
}
#endif

#endif
