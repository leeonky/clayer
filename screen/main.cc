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
	return video_event(iob, [&](int fw, int fh, enum AVPixelFormat av_format){
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
					return buffer_event(iob, [&](int shmid, size_t size, int count, int semid) {
						return circular_shm::load(shmid, size, count, semid,
							[&](circular_shm &shm){
							media_clock clock;
							while((!frames_event(iob, [&](frame_list &frames){
									for(int i=0; i<frames.count; i++){
										shm.free(frames.frames[i].index, [&](void *buffer){
											return av_image_fill_arrays(fw, fh, av_format, buffer, [&](uint8_t **datas, int *lines){
												clock.wait(frames.frames[i].timestamp, 100000);
												SDL_RenderClear(renderer);
												return SDL_PresentYUV(renderer, texture, datas, lines);
												});
											});
									}
									return 0;
									})) || (!clock_event(iob, [&](int64_t base, int64_t offset){
										clock.sync(base, offset);
										return 0;
										})))
							;
							return 0;
							});
						});
					});	
				});
			});
}
