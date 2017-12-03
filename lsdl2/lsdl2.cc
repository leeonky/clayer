#include "lsdl2.h"
#include "stdexd/stdexd.h"

#define log_sdl_error() log_error("liblsdl2", "%s", SDL_GetError())

int SDL_CreateWindow(const char *caption,
		int x, int y, int width, int height, Uint32 flag,
		const std::function<int(SDL_Window *)> &action) {
	int res = 0;
	if(!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		if(SDL_Window *window = SDL_CreateWindow(caption, x, y, width, height, flag)) {
			SDL_ShowCursor(SDL_DISABLE);
			res = action(window);
			SDL_DestroyWindow(window);
		} else
			res = log_sdl_error();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	} else
		res = log_sdl_error();
	return res;
}
