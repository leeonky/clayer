#include "mock_sdl2.h"

mock_function_1(int, SDL_InitSubSystem, Uint32);
mock_void_function_1(SDL_QuitSubSystem, Uint32);

mock_function_3(SDL_Renderer *, SDL_CreateRenderer, SDL_Window *, int, Uint32);
mock_void_function_1(SDL_DestroyRenderer, SDL_Renderer *);

mock_function_6(SDL_Window *, SDL_CreateWindow, const char *, int, int, int, int, Uint32);
mock_void_function_1(SDL_DestroyWindow, SDL_Window *);

mock_function_5(SDL_Texture *, SDL_CreateTexture, SDL_Renderer *, Uint32, int, int, int);
mock_void_function_1(SDL_DestroyTexture, SDL_Texture *);


mock_function_1(int, SDL_ShowCursor, int);
mock_void_function_3(SDL_GL_GetDrawableSize, SDL_Window *, int *, int *);

mock_function_8(int, SDL_UpdateYUVTexture, SDL_Texture *, const SDL_Rect *, const Uint8 *, int , const Uint8 *, int , const Uint8 *, int);
mock_function_4(int, SDL_RenderCopy, SDL_Renderer *, SDL_Texture *, const SDL_Rect *, const SDL_Rect *);
mock_void_function_1(SDL_RenderPresent, SDL_Renderer *);

mock_function_5(SDL_AudioDeviceID, SDL_OpenAudioDevice, const char *, int, const SDL_AudioSpec *, SDL_AudioSpec *, int);
mock_void_function_1(SDL_CloseAudioDevice, SDL_AudioDeviceID);
mock_function_2(const char *, SDL_GetAudioDeviceName, int, int);
mock_void_function_2(SDL_PauseAudioDevice, SDL_AudioDeviceID, int);
mock_function_3(int, SDL_QueueAudio, SDL_AudioDeviceID, const void *, Uint32);

mock_function_1(Uint32, SDL_GetQueuedAudioSize, SDL_AudioDeviceID);

const char *SDL_GetError() {
	return "sdl error";
}
