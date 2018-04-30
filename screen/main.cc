#include <map>
#include <memory>
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

namespace {
	class TextureDeleter {
	public:
		void operator() (SDL_Texture *t) {
			if(t)
				SDL_DestroyTexture(t);
		}
	};

	Uint32 window_flag = 0;
	char title[128] = "CLAYER";
	int x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED, win_w, win_h, frm_w, frm_h;
	enum AVPixelFormat format;
	circular_shm *shms[MAX_LAYER_COUNT];
	using SDL_TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
	std::map<int, SDL_TexturePtr> layer_textures;

	void process_args(int argc, char **argv) {
		command_argument().require_full_argument("position", 'p', [&](const char *arg){
				sscanf(arg, "%d,%d", &x, &y);
				})
		.require_full_argument("size", 's', [&](const char *arg){
				sscanf(arg, "%dx%d", &win_w, &win_h);
				})
		.require_full_argument("flag", 'f', [&](const char *arg){
				if(strstr(arg, "full"))
					window_flag |= SDL_WINDOW_FULLSCREEN;
				if(strstr(arg, "opengl"))
					window_flag |= SDL_WINDOW_OPENGL;
				if(strstr(arg, "borderless"))
					window_flag |= SDL_WINDOW_BORDERLESS;
				if(strstr(arg, "highdpi"))
					window_flag |= SDL_WINDOW_ALLOW_HIGHDPI;
				}).parse(argc, argv);
	}

	inline std::function<int(int64_t, int64_t)> clock_action(media_clock &clock) {
		return [&](int64_t base, int64_t offset){
			clock.sync(base, offset);
			return 0;
		};
	}

	inline std::function<int(const layer_list &layer)> layer_action(SDL_Renderer *renderer, int fw, int fh, std::map<int, SDL_TexturePtr> &layer_textures) {
		return [&, renderer, fw, fh](const layer_list &layer){
			shms[layer.buffer_key]->free(layer.index, [&](void *layer_buffer){
				if(SDL_Texture *t = SDL_CreateTexture(renderer,
							SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fw, fh)) {
					layer_textures[layer.id] = SDL_TexturePtr(t);
					SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
					for(int i=0; i<layer.count; ++i) {
						SDL_Rect rect = {
							layer.sub_layers[i].x, layer.sub_layers[i].y,
							layer.sub_layers[i].w, layer.sub_layers[i].h
						};
						SDL_UpdateTexture(t, &rect, ((char *)layer_buffer)+layer.sub_layers[i].offset,
								layer.sub_layers[i].pitch);
					}
				}
			});
			return 0;
		};
	}

	inline std::function<int(int)> no_layer_action(std::map<int, SDL_TexturePtr> &layer_textures) {
		return [&](int id) {
			layer_textures.erase(id);
			return 0;
		};
	}
	inline std::function<int(void)> reset_action(player_context &context) {
		return [&]() {
			context.reset();
			return 0;
		};
	}

	inline std::function<int(int, int, int64_t)> play_frame(iobus &iob, player_context &context, SDL_Renderer *renderer, SDL_Texture *texture) {
		return [&, renderer, texture](int buffer_key, int index, int64_t pts){
			context.process_command();
			shms[buffer_key]->free(index, [&](void *buffer){
					return av_image_fill_arrays(frm_w, frm_h, format, buffer, [&](uint8_t **datas, int *lines){
							bool show_pic = !context.is_resetting();

							SDL_RenderClear(renderer);
							SDL_UpdateAndCopy(renderer, texture, datas, lines);

							while(!iob.except("FRAME"))
								if(clock_event(iob, clock_action(context.clock()))
									&& layer_event(iob, layer_action(renderer, frm_w, frm_h, layer_textures))
									&& nolayer_event(iob, no_layer_action(layer_textures))
									&& reset_event(iob, reset_action(context))
									&& iob.ignore_last())
									break;

							for(const auto &key_layer : layer_textures)
								SDL_RenderCopy(renderer, key_layer.second.get(),  NULL, NULL);

							if(show_pic) {
								context.sync_clock_as_needed(pts);
								context.clock().wait(pts, 100000);
								SDL_RenderPresent(renderer);
							}
							return 0;
					});
			});
			return 0;
		};
	}
}

int main(int argc, char **argv) {
	iobus iob(stdin, stdout, stderr);
	return player_context::start(iob, [&](player_context &context) {
			return forward_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat av_format){
					win_w = frm_w = fw;
					win_h = frm_h = fh;
					format = av_format;
					process_args(argc, argv);
					if(window_flag & SDL_WINDOW_ALLOW_HIGHDPI) {
						win_w /= 2;
						win_h /= 2;
					}
					return SDL_CreateWindow(title, x, y, win_w, win_h, window_flag,
							[&](SDL_Window *window){
							SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "4");
							return SDL_CreateTexture(window, frm_w, frm_h, AVPixelFormat_to_SDL(av_format),
									[&](int, int, SDL_Renderer *renderer, SDL_Texture *texture){
									SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
									SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
									SDL_SetRelativeMouseMode(SDL_TRUE);
									return main_reducer(iob, shms, frame_event, play_frame(iob, context, renderer, texture));
									});
							});
					});
			});
}
