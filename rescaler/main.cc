#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

namespace {
	circular_shm *shms[MAX_LAYER_COUNT];
	int new_key = 1;
	int new_count = 16;

	int arg_w, arg_h;
	const char *from_format = "";
	const char *to_format = "";
	enum AVPixelFormat out_format;


	void process_arg(int argc, char **argv) {
		command_argument().require_full_argument("form_format", 't', [&](const char *arg){
				from_format = arg;
				}).require_full_argument("to_format", 'f', [&](const char *arg){
					to_format = arg;
					}).require_full_argument("size", 's', [&](const char *arg){
						analyze_size_format(arg_w, arg_h, arg);
						}).parse(argc, argv);
	}
}

int main(int argc, char **argv) {
	iobus iob(stdin, stdout, stderr);

	return forward_untill(iob, video_event, [&](int w, int h, enum AVPixelFormat format){
			arg_w = w;
			arg_h = h;
			process_arg(argc, argv);
			out_format = analyze_pixel_format(format, from_format, to_format);
			if(w==arg_w && h==arg_h && out_format==format) {
				iob.recaption_and_post();
				while(!iob.pass_through());
				return -1;
			}
			return sws_getContext(w, h, format, arg_w, arg_h, out_format, SWS_FAST_BILINEAR, [&](scale_context &context) {
					iob.post("VIDEO width:%d height:%d format:%s", arg_w, arg_h, av_get_pix_fmt_name(out_format));
					return circular_shm::create(context.rescale_size(), new_count,
							[&](circular_shm &shm){
							iob.post("%s", shm.serialize_to_string(new_key));
							return main_transform(iob, shms, frame_event, [&](int buffer_key, int index, int64_t pts) {
									shms[buffer_key]->free(index, [&](void *buffer){
											if(!sws_scale(context, buffer, shm.allocate()))
													iob.post("FRAME buffer:%d %d=>%" PRId64, new_key, shm.index, pts);
											});
									return 0;
									});
							});
					});
	});
}

