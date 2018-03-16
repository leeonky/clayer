#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"

int main(int argc, char **argv) {
	int w=-1, h=-1;
	int new_key = 2;
	int new_count = 16;
	int layer_id = 0;
	char font_file[512] = {};
	circular_shm *shms[MAX_LAYER_COUNT];
	const char *file_name = command_argument().require_full_argument("size", 's', [&](const char *arg){
			sscanf(arg, "%dx%d", &w, &h);
			}).require_full_argument("font", 'f', [&](const char *arg){
			sscanf(arg, "%s", font_file);
			}).parse(argc, argv);

	if(!file_name) {
		fprintf(app_stderr, "Error[subtitle]: require subtitle file\n");
		return -1;
	}
	if(!strlen(font_file)) {
		fprintf(app_stderr, "Error[subtitle]: require font file\n");
		return -1;
	}

	return fopen(file_name, "rb", [&](FILE *sub_file){
		iobus iob(stdin, stdout, stderr);

		return ignore_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat /*av_format*/){
				w = w==-1 ? fw : w;
				h = h==-1 ? fh : h;
				iob.recaption_and_post();

				return circular_shm::create(w*h*4, new_count,
						[&](circular_shm &shm){
						iob.post("%s", shm.serialize_to_string(new_key));
						return TTF_OpenFont(font_file, 60, [&](TTF_Font *font) {
								return main_transform(iob, shms, frame_event, [&](int, int, int64_t pts) {
										iob.recaption_and_post();
										SDL_Color color = {255, 255, 255, 0};
										char *buffer = (char *)shm.allocate();
										TTF_RenderUTF8_Blended(font, "你好中国", color, [&](SDL_Surface *surface) {
												SDL_LockSurface(surface);

												memcpy(buffer, surface->pixels, surface->pitch*surface->h);
												iob.post("LAYER buffer:%d index:%d id:%d 0=>%d,%d,%d,%d,%d", new_key, shm.index, layer_id, 0, 0, surface->w, surface->h, surface->pitch);
												SDL_UnlockSurface(surface);
												return 0;
												});

										TTF_RenderUTF8_Blended(font, "测试字母", color, [&](SDL_Surface *surface) {
												SDL_LockSurface(surface);

												memcpy(buffer+1024000, surface->pixels, surface->pitch*surface->h);
												iob.post("LAYER buffer:%d index:%d id:%d 1024000=>%d,%d,%d,%d,%d", new_key, shm.index, layer_id, 200, 200, surface->w, surface->h, surface->pitch);
												SDL_UnlockSurface(surface);
												return 0;
												});
										return 0;
										});

								});

						});
				});

		});
}

