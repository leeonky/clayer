#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

int main(int, char **) {
	iobus iob(stdin, stdout, stderr);
	return video_event(iob, [&](int fw, int fh, enum AVPixelFormat av_format){
			return SDL_CreateWindow("", 0, 0, 1920, 1080, 0,
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
