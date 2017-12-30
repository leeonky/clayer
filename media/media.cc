#include <cinttypes>
#include <unistd.h>
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
			log_error("Unknow support ffmpeg video format '%d'", format);
	  		return SDL_PIXELFORMAT_UNKNOWN;
	}
}

SDL_AudioFormat AVSampleFormat_to_SDL(enum AVSampleFormat format) {
	switch(format) {
		case AV_SAMPLE_FMT_U8:
			return AUDIO_U8;
		case AV_SAMPLE_FMT_S16:
			return AUDIO_S16;
		case AV_SAMPLE_FMT_S32:
			return AUDIO_S32;
		case AV_SAMPLE_FMT_FLT:
			return AUDIO_F32;
		default:
			log_error("Unknow support ffmpeg audio format '%d'", format);
	  		return 0;
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
				res = log_error("Unsupport ffmpeg video format '%s'", format);
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
	return iob.get("FRAMES", [&](const char *, const char *arguments) {
			arguments = strlen(arguments)==0 ? " " : arguments;
			return fmemopen((void *)arguments, strlen(arguments), "r", [&](FILE *file) {
				frame_list list;
				while(list.count<MAX_FRAMES_COUNT
					&& 2==fscanf(file, "%d=>%" PRId64, &list.frames[list.count].index, &list.frames[list.count].timestamp))
					list.count++;
				return action(list);
				});
			});
}

int samples_event(iobus &iob, const std::function<int(sample_list &)> &action) {
	return iob.get("SAMPLES", [&](const char *, const char *arguments) {
			arguments = strlen(arguments)==0 ? " " : arguments;
			return fmemopen((void *)arguments, strlen(arguments), "r", [&](FILE *file) {
				sample_list list;
				while(list.count<MAX_SAMPLES_COUNT
					&& 3==fscanf(file, "%d=>%" PRId64 ",%d", &list.samples[list.count].index, &list.samples[list.count].timestamp, &list.samples[list.count].nb_samples))
					list.count++;
				return action(list);
				});
			});
}

int audio_event(iobus &iob, const std::function<int(int, int, int64_t, enum AVSampleFormat)> &action) {
	int rate, channels;
	char layout[128], format[128];
	return iob.get("AUDIO", [&](const char *, const char *) {
			int res = 0;
			enum AVSampleFormat av_format = av_get_sample_fmt(format);
			if(int64_t av_layout = av_get_channel_layout(layout)) {
				if(AV_SAMPLE_FMT_NONE == av_format)
					res = log_error("Unsupport ffmpeg audio format '%s'", format);
				else
					res = action(rate, channels, av_layout, av_format);
			} else
				res = log_error("Unsupport ffmpeg audio layout '%s'", layout);
			return res;
			}, 4, "sample_rate:%d channels:%d layout:%s format:%s", &rate, &channels, layout, format);
}

int media_clock::wait(int64_t pts, int64_t period) {
	int64_t s = pts - offset - (usectime()-base);
	if(s<0)
		return -1;
	if(s>period)
		usleep(period);
	else if(s)
		usleep(s);
	return 0;
}

void wait_at_least(int64_t waiting, int64_t least) {
	waiting -= least;
	if (waiting>0)
		usleep(waiting);
}

