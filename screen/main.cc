#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

int main(int argc, char **argv) {
	Uint32 window_flag = 0;
	int x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;
	iobus iob(stdin, stdout, stderr);
	circular_shm *shms[MAX_LAYER_COUNT];

	auto video_action = [&](int fw, int fh, enum AVPixelFormat av_format){
			int w = fw, h = fh;
			char title[128] = "CLAYER";

			command_argument().require_full_argument("position", 'p', [&](const char *arg){
					sscanf(arg, "%d,%d", &x, &y);
					})
			.require_full_argument("size", 's', [&](const char *arg){
					sscanf(arg, "%dx%d", &w, &h);
					})
			.require_full_argument("flag", 'f', [&](const char *arg){
					if(strstr(arg, "full"))
						window_flag |= SDL_WINDOW_FULLSCREEN;
					if(strstr(arg, "opengl"))
						window_flag |= SDL_WINDOW_OPENGL;
					if(strstr(arg, "borderless"))
						window_flag |= SDL_WINDOW_BORDERLESS;
					}).parse(argc, argv);

			return SDL_CreateWindow(title, x, y, w, h, window_flag,
				[&](SDL_Window *window){
				return SDL_CreateTexture(window, fw, fh, AVPixelFormat_to_SDL(av_format),
					[&](int, int, SDL_Renderer *renderer, SDL_Texture *texture){
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					media_clock clock;
					return main_consumer(iob, shms, frame_event, [&](int buffer_key, int index, int64_t pts){
						shms[buffer_key]->free(index, [&](void *buffer){
							return av_image_fill_arrays(fw, fh, av_format, buffer, [&](uint8_t **datas, int *lines){
								SDL_RenderClear(renderer);
								SDL_UpdateAndCopyYUV(renderer, texture, datas, lines);
								while(!iob.except("FRAME")) {
									if(clock_event(iob, [&](int64_t base, int64_t offset){
										clock.sync(base, offset);
										return 0;
										})
										&& iob.ignore_last())
										break;
								}

								SDL_RenderPresent(renderer);
								clock.wait(pts, 100000);
								return 0;
								});
							});
						return 0;
						});
					});
				});
			};

	return ignore_untill(iob, video_event, video_action);
}
