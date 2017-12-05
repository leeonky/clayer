#include "media.h"
#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"

#undef log_error
#define log_error(format, ...) log_error("libmedia", (format), ## __VA_ARGS__)

Uint32 AVPixelFormat_to_SDL(const char *format) {
	switch(av_get_pix_fmt(format)) {
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUV444P:
		case AV_PIX_FMT_YUV410P:
		case AV_PIX_FMT_YUV411P:
			return SDL_PIXELFORMAT_IYUV;
		case AV_PIX_FMT_NONE:
			log_error("Unknow ffmpeg format '%s'", format);
	  		return SDL_PIXELFORMAT_UNKNOWN;
		default:
			log_error("Unknow support format '%s'", format);
	  		return SDL_PIXELFORMAT_UNKNOWN;
	}
}

int open_video(iobus &iob, const char *caption, int x, int y, int width, int height, Uint32 flag, const std::function<int(SDL_Window *, SDL_Renderer *, int, int, SDL_Texture *)> &action) {
	return iob.get([&](const char *command, const char *arguments){
			int res = 0;
			if(!strcmp(command, "VIDEO")) {
				int vw, vh;
				char format [128] = "";
				if(3==sscanf(arguments, "width:%d height:%d format:%s", &vw, &vh, format)){
					Uint32 sdl_format = AVPixelFormat_to_SDL(format);
					if(sdl_format != SDL_PIXELFORMAT_UNKNOWN) {
						res = SDL_CreateWindow(caption, x, y, width, height, flag, [&](SDL_Window *window){
							return SDL_CreateTexture(window, vw, vh, sdl_format, [&](int w, int h, SDL_Renderer *renderer, SDL_Texture *texture){
								return action(window, renderer, w, h, texture);
								});
							});
					} else
						res = -1;
				} else
					res = log_error("Invalid VIDEO arguments '%s'", arguments);
			} else
				res = -1;
			return res;
			});
}

int load_buffer(iobus &iob, const std::function<int(circular_shm &)> &action) {
	return iob.get([&](const char *command, const char *arguments){
			int res = 0;
			int shm_id, sem_id, count;
			size_t element_size;
			sscanf(arguments, "id:%d size:%zu count:%d sem:%d", &shm_id, &element_size, &count, &sem_id);
			return circular_shm::load(shm_id, element_size, count, sem_id, action);
			});
}

