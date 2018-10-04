#include <map>
#include <memory>
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"

namespace {
	circular_shm *shms[MAX_LAYER_COUNT];
}

int main(int argc, char **argv) {
	iobus iob(stdin, stdout, stderr);
	return player_context::start(iob, [&](player_context &context) {
			return ignore_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat av_format){
					return main_reducer(iob, shms, frame_event, [&](int buffer_key, int index, int64_t pts){
							shms[buffer_key]->free(index, [&](void *){
									printf("Frame: %3d pts: %10" PRId64 " now: %10" PRId64 " key: %d format: %dx%d %s\n",
											index, pts, context.clock().now(), buffer_key, fw, fh, av_get_pix_fmt_name(av_format));
									});
							return 0;
							});
					});
			});
}

