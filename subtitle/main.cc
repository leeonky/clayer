#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"
#include "media/sub_srt.h"
#include "media/sub_ass.h"
#include <sstream>
#include <iostream>
#include <vector>

namespace {
	int w=-1, h=-1;
	int subtitle_buffer_key = 2;
	int subtitle_buffer_count = 4;
	int layer_id = 0;
	char font_file[512] = {};
	circular_shm *shms[MAX_LAYER_COUNT];
	int subtitle_bottom_pos = -5;
	int font_size_k = 50;
	const char *file_name;
	SDL_Color srt_subtitle_color = {255, 255, 255, 0};

	void process_args(int argc, char **argv) {
		file_name = command_argument().require_full_argument("size", 's', [&](const char *arg){
				sscanf(arg, "%dx%d", &w, &h);
				}).require_full_argument("font", 'f', [&](const char *arg){
					sscanf(arg, "%s", font_file);
					}).parse(argc, argv);

		if(!file_name) {
			fprintf(app_stderr, "Error[subtitle]: require subtitle file\n");
			exit(-1);
		}
		if(!strlen(font_file)) {
			fprintf(app_stderr, "Error[subtitle]: require font file\n");
			exit(-1);
		}
	}

	size_t render_lines(iobus &iob, std::istringstream &lines, uint8_t *buffer, TTF_Font *font, const SDL_Color &color, int &h) {
		std::string line;
		if(!std::getline(lines, line))
			return 0;

		size_t offset = render_lines(iob, lines, buffer, font, color, h);
		TTF_RenderUTF8_Blended(font, line.c_str(), color, [&](SDL_Surface *surface) {
				SDL_LockSurface(surface);
				size_t size = surface->pitch*surface->h;
				memcpy(buffer+offset, surface->pixels, size);
				h -= surface->h;
				iob.post_some(" %d=>%d,%d,%d,%d,%d",
						offset, (w-surface->w)/2, h,
						surface->w, surface->h, surface->pitch);
				offset += size;
				SDL_UnlockSurface(surface);
				return 0;
				});
		return offset;
	}

	inline std::function<void(const std::string &)> subtitle_action(iobus &iob, circular_shm &shm, TTF_Font *font, const SDL_Color &color) {
		return [&, font](const std::string &title) {
			if(!title.empty()) {
				std::istringstream f(title);
				uint8_t *buffer = (uint8_t *)shm.allocate();
				iob.post_some("LAYER buffer:%d index:%d id:%d width:%d height:%d", subtitle_buffer_key, shm.index, layer_id, w, h);
				int title_h = ((100+subtitle_bottom_pos)%100)*h/100;
				render_lines(iob, f, buffer, font, color, title_h);
				iob.post("");
			} else
				iob.post("NOLAYER id:%d", layer_id);
		}; 
	}

	inline std::function<int(FILE *)> process_srt(iobus &iob, circular_shm &shm) {
		return [&](FILE *sub_file){
			subtitle_srt srt(sub_file);
			return TTF_OpenFont(font_file, w/font_size_k, [&](TTF_Font *font) {
					return main_transform(iob, shms, frame_event, [&](int, int, int64_t pts) {
							iob.recaption_and_post();
							srt.query_item(pts, subtitle_action(iob, shm, font, srt_subtitle_color));
							return 0;
							});

					});
		};
	}

	inline size_t pixel_copy(uint8_t *buffer, ASS_Image *image) {
		uint32_t *colors = (uint32_t *)buffer;
		uint32_t color = ((image->color>>8) & 0xffffff);
		uint8_t *bitmap = image->bitmap;

		for(int y=0; y<image->h; ++y) {
			for(int x=0; x<image->w; ++x)
				colors[x] = ((bitmap[x] << 24) & 0xff000000) + color;
			colors += image->stride;
			bitmap += image->stride;
		}
		return image->stride*4 * image->h;
	}

	inline int process_ass(iobus &iob, circular_shm &shm) {
		return subtitle_ass(file_name, w, h, font_file, [&](ASS_Renderer *renderer, ASS_Track *track){
				return main_transform(iob, shms, frame_event, [&](int, int, int64_t pts) {
						iob.recaption_and_post();
						ass_render_frame(renderer, track, pts, [&](ASS_Image *image) {
								if(image) {
									uint8_t *buffer = (uint8_t *)shm.allocate();
									iob.post_some("LAYER buffer:%d index:%d id:%d width:%d height:%d", subtitle_buffer_key, shm.index, layer_id, w, h);
									for(int offset=0; image; image = image->next) {
										size_t size = pixel_copy(buffer+offset, image);
										iob.post_some(" %d=>%d,%d,%d,%d,%d",
												offset, image->dst_x, image->dst_y,
												image->w, image->h, image->stride*4);
										offset += size;
									}
									iob.post("");
								} else
									iob.post("NOLAYER id:%d", layer_id);
								});
						return 0;
						});
				});
	}
}

int main(int argc, char **argv) {
	process_args(argc, argv);
	iobus iob(stdin, stdout, stderr);
	const char *file_ext = strrchr(file_name, '.');
	return forward_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat /*av_format*/){
			w = w==-1 ? fw : w;
			h = h==-1 ? fh : h;
			iob.recaption_and_post();
			return circular_shm::create(w*h*4, subtitle_buffer_count,
					[&](circular_shm &shm){
					iob.post("%s", shm.serialize_to_string(subtitle_buffer_key));
					return !strcmp(file_ext, ".ass") ? process_ass(iob, shm) : fopen(file_name, "rb", process_srt(iob, shm));
					});

			});
}

