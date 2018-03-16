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

int SDL_CreateTexture(SDL_Window *window, int width, int height, Uint32 format, const std::function<int(int, int, SDL_Renderer *, SDL_Texture *)> &action) {
	int res = 0;
	SDL_Renderer *renderer;
       	//int w, h;
	if((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))
			|| (renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE))) {
		//SDL_GL_GetDrawableSize(window, &w, &h);
		//w = w>width ? width : w;
		//h = h>height ? height : h;
		//if(SDL_Texture *texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, w, h)) {
			//res = action(w, h, renderer, texture);
			//SDL_DestroyTexture(texture);
		//} else
		if(SDL_Texture *texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, width, height)) {
			res = action(width, height, renderer, texture);
			SDL_DestroyTexture(texture);
		} else
			res = log_sdl_error();
		SDL_DestroyRenderer(renderer);
	} else
		res = log_sdl_error();
	return res;
}

int SDL_UpdateAndCopyYUV(SDL_Renderer *renderer, SDL_Texture *texture, uint8_t **datas, int *lines) {
	int res = 0;
	SDL_Rect target_rect;
	int renderer_w, renderer_h, texture_w, texture_h;
	if(!SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h)
			&& !SDL_QueryTexture(texture, NULL, NULL, &texture_w, &texture_h)) {
		float renderer_w_h_radio = static_cast<float>(renderer_w)/renderer_h;
		float texture_w_h_radio = static_cast<float>(texture_w)/texture_h;

		if(renderer_w_h_radio > texture_w_h_radio) {
			target_rect.w = renderer_h*texture_w/texture_h;
			target_rect.h = renderer_h;
			target_rect.x = (renderer_w-target_rect.w)/2;
			target_rect.y = 0;
		} else {
			target_rect.w = renderer_w;
			target_rect.h = renderer_w*texture_h/texture_w;
			target_rect.x = 0;
			target_rect.y = (renderer_h-target_rect.h)/2;
		}
		if(SDL_UpdateYUVTexture(texture, NULL, datas[0], lines[0], datas[1], lines[1], datas[2], lines[2])
				|| SDL_RenderCopy(renderer, texture,  NULL, &target_rect))
			res = log_sdl_error();
	} else
		res = log_sdl_error();
	return res;
}

int SDL_PresentYUV(SDL_Renderer *renderer, SDL_Texture *texture, uint8_t **datas, int *lines) {
	int res = 0;
	if(!SDL_UpdateAndCopyYUV(renderer, texture, datas, lines))
		SDL_RenderPresent(renderer);
	else
		res = -1;
	return res;
}

int SDL_OpenAudio(int index, int freq, int channels, SDL_AudioFormat format, const std::function<int(SDL_AudioDeviceID, const SDL_AudioSpec &)> &action) {
	int res;
	if(!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
		if(const char *device_name = SDL_GetAudioDeviceName(index, 0)) {
			SDL_AudioSpec obtained;
			SDL_AudioSpec desired;
			memset(&obtained, 0, sizeof(SDL_AudioSpec));
			memset(&desired, 0, sizeof(SDL_AudioSpec));
			desired.freq = freq;
			desired.channels = channels;
			desired.format = format;
			if(SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(device_name, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) {
				if(desired.freq != obtained.freq || desired.channels != obtained.channels || desired.format != obtained.format)
					log_warning("liblsdl2", "audio parameters changed");
				res = action(device_id, obtained);
				SDL_CloseAudioDevice(device_id);
			} else
				res = log_sdl_error();
		} else
			res = log_sdl_error();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	} else
		res = log_sdl_error();
	return res;
}

int SDL_QueueAudio(SDL_AudioDeviceID device_id, const void *buffer, int channels, SDL_AudioFormat format, int samples) {
	return SDL_QueueAudio(device_id, buffer, channels*samples*(SDL_AUDIO_BITSIZE(format)>>3));
}

int SDL_AudioLast(SDL_AudioDeviceID device_id, const SDL_AudioSpec &spec) {
	int64_t left = SDL_GetQueuedAudioSize(device_id);
	return left*1000000/(spec.channels*(SDL_AUDIO_BITSIZE(spec.format)>>3))/spec.freq;
}

int TTF_OpenFont(const char *file, int size, const std::function<int(TTF_Font *)> &action) {
	int ret = 0;
	if(!TTF_Init()) {
		if(TTF_Font *font = TTF_OpenFont(file, size)) {
			ret = action(font);
			TTF_CloseFont(font);
		} else
			ret = log_sdl_error();
		TTF_Quit();
	} else
		ret = log_sdl_error();
	return ret;
}

int TTF_RenderUTF8_Blended(TTF_Font *font, const char *text, SDL_Color fg, const std::function<int(SDL_Surface *)> &action) {
	int ret = 0;
	if(SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, fg)) {
		ret = action(surface);
		SDL_FreeSurface(surface);
	} else
		ret = log_sdl_error();
	return ret;
}

