#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"
#include "media/sub_srt.h"

namespace {
	int subtitle_bottom_pos = -5;

	inline int h_pos(int height, int sub_height) {
		return ((100+subtitle_bottom_pos)%100)*height/100-sub_height;
	}
}

int main(int argc, char **argv) {
	int w=-1, h=-1;
	int subtitle_buffer_key = 2;
	int subtitle_buffer_count = 4;
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
		subtitle_srt srt(sub_file);
		iobus iob(stdin, stdout, stderr);
		return ignore_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat /*av_format*/){
				w = w==-1 ? fw : w;
				h = h==-1 ? fh : h;
				iob.recaption_and_post();
				return circular_shm::create(w*h*4, subtitle_buffer_count,
						[&](circular_shm &shm){
						iob.post("%s", shm.serialize_to_string(subtitle_buffer_key));
						return TTF_OpenFont(font_file, 60, [&](TTF_Font *font) {
								SDL_Color subtitle_color = {255, 255, 255, 0};
								return main_transform(iob, shms, frame_event, [&](int, int, int64_t pts) {
										iob.recaption_and_post();
										srt.query_item(pts, [&](const std::string &title) {
												if(!title.empty())
													TTF_RenderUTF8_Blended(font, title.c_str(), subtitle_color, [&](SDL_Surface *surface) {
															SDL_LockSurface(surface);

															memcpy(shm.allocate(), surface->pixels, surface->pitch*surface->h);
															iob.post("LAYER buffer:%d index:%d id:%d 0=>%d,%d,%d,%d,%d", subtitle_buffer_key, shm.index, layer_id, (w-surface->w)/2, h_pos(h, surface->h), surface->w, surface->h, surface->pitch);
															SDL_UnlockSurface(surface);
															return 0;
															});
												else
													iob.post("NOLAYER id:%d", layer_id);

												});

										return 0;
										});

								});

						});
				});

		});
}

