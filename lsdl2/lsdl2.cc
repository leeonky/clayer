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

int SDL_CreateTexture(SDL_Window *window, int width, int height, Uint32 format, const std::function<int(int, int, SDL_Texture *)> &action) {
	int res = 0, w, h;
	if(SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) {
		SDL_GL_GetDrawableSize(window, &w, &h);
		w = w>width ? width : w;
		h = h>height ? height : h;
		if(SDL_Texture *texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, w, h)) {
			res = action(w, h, texture);
			SDL_DestroyTexture(texture);
		} else
			res = log_sdl_error();
		SDL_DestroyRenderer(renderer);
	} else
		res = log_sdl_error();
	return res;
}

