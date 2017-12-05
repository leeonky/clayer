#include "media.h"
#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"

#undef log_error
#define log_error(format, ...) log_error("libmedia", (format), ## __VA_ARGS__)

Uint32 AVPixelFormat_to_SDL(enum AVPixelFormat format) {

	switch(format) {
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUV444P:
		case AV_PIX_FMT_YUV410P:
		case AV_PIX_FMT_YUV411P:
			return SDL_PIXELFORMAT_IYUV;
		default:
			log_error("Unknow support format '%d'", format);
	  		return SDL_PIXELFORMAT_UNKNOWN;
	}
}

int video_event(iobus &iob, const std::function<int(int, int, enum AVPixelFormat)> &action) {
	int vw, vh;
	char format [128] = "";
	return iob.get("VIDEO", [&](const char *, const char *) {
			int res;
			enum AVPixelFormat av_format = av_get_pix_fmt(format);
			if(av_format != AV_PIX_FMT_NONE)
				res = action(vw, vh, av_format);
			else
				res = log_error("Unsupport ffmpeg format '%s'", format);
			return res;
			}, 3, "width:%d height:%d format:%s", &vw, &vh, format);
}

int buffer_event(iobus &iob, const std::function<int(int, size_t, int, int)> &action) {
	int shm_id, sem_id, count;
	size_t element_size;
	return iob.get("BUFFER", [&](const char *, const char *) {
			return action(shm_id, element_size, count, sem_id);
			}, 4, "id:%d size:%zu count:%d sem:%d", &shm_id, &element_size, &count, &sem_id);
}

int frames_event(iobus &iob, const std::function<int(frame_list &)> &action) {
	return iob.get("FRAMES", [&](const char *, const char *) {
			frame_list list;
			return action(list);
			});
}

