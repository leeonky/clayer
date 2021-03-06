#include <regex>
#include <numeric>
#include <algorithm>
#include <cinttypes>
#include <unistd.h>
#include "media.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
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
		case AV_PIX_FMT_RGB24:
			return SDL_PIXELFORMAT_RGB24;
		case AV_PIX_FMT_BGR24:
			return  SDL_PIXELFORMAT_BGR24;
		case AV_PIX_FMT_ARGB:
			return SDL_PIXELFORMAT_ARGB8888;
		case AV_PIX_FMT_RGBA:
			return SDL_PIXELFORMAT_RGBA8888;
		case AV_PIX_FMT_ABGR:
			return SDL_PIXELFORMAT_ABGR8888;
		case AV_PIX_FMT_BGRA:
			return SDL_PIXELFORMAT_BGRA8888;
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

PaSampleFormat AVSampleFormat_to_PortAudio(enum AVSampleFormat format) {
	switch(format) {
		case AV_SAMPLE_FMT_U8:
			return paUInt8;
		case AV_SAMPLE_FMT_S16:
			return paInt16;
		case AV_SAMPLE_FMT_S32:
			return paInt32;
		case AV_SAMPLE_FMT_FLT:
			return paFloat32;
		default:
			log_error("Unknow support ffmpeg audio format '%d'", format);
			return 0;
	}
}

snd_pcm_format_t AVSampleFormat_to_ALSA(enum AVSampleFormat format) {
	switch(format) {
		case AV_SAMPLE_FMT_U8:
			return SND_PCM_FORMAT_U8;
		case AV_SAMPLE_FMT_S16:
			return SND_PCM_FORMAT_S16_LE;
		case AV_SAMPLE_FMT_S32:
			return SND_PCM_FORMAT_S32_LE;
		case AV_SAMPLE_FMT_FLT:
			return SND_PCM_FORMAT_FLOAT_LE;
		case AV_SAMPLE_FMT_DBL:
			return SND_PCM_FORMAT_FLOAT64_LE;
		default:
			log_error("Unknow support ffmpeg audio format '%d'", format);
			return SND_PCM_FORMAT_UNKNOWN;
	}
}

int video_event(iobus &iob, const std::function<int(int, int, enum AVPixelFormat)> &action) {
	int vw, vh;
	char format [128] = "";
	return iob.get("VIDEO", [&] {
			int res;
			enum AVPixelFormat av_format = av_get_pix_fmt(format);
			if(av_format != AV_PIX_FMT_NONE)
				res = action(vw, vh, av_format);
			else
				res = log_error("Unsupport ffmpeg video format '%s'", format);
			return res;
			}, 3, "width:%d height:%d format:%s", &vw, &vh, format);
}

int buffer_event(iobus &iob, const std::function<int(int, size_t, int, int, int)> &action) {
	int shm_id, sem_id, count, index;
	size_t element_size;
	return iob.get("BUFFER", [&] {
			return action(shm_id, element_size, count, sem_id, index);
			}, 5, "id:%d size:%zu count:%d sem:%d key:%d", &shm_id, &element_size, &count, &sem_id, &index);
}

int frames_event(iobus &iob, const std::function<int(frame_list &)> &action) {
	return iob.get("FRAMES", [&](const char *arguments) {
			arguments = strlen(arguments)==0 ? " " : arguments;
			return fmemopen((void *)arguments, strlen(arguments), "r", [&](FILE *file) {
				frame_list list = {};
				if(1==fscanf(file, "buffer:%d", &list.buffer_key)) {
					while(list.count<MAX_FRAMES_COUNT
						&& 2==fscanf(file, "%d=>%" PRId64, &list.frames[list.count].index, &list.frames[list.count].timestamp))
						list.count++;
				}
				return action(list);
				});
			});
}

int frame_event(iobus &iob, const std::function<int(int, int, int64_t)> &action) {
	int buffer_key, index;
	int64_t pts;
	return iob.get("FRAME", [&] {
			return action(buffer_key, index, pts);
			}, 3, "buffer:%d %d=>%" PRId64, &buffer_key, &index, &pts);
}

int samples_event(iobus &iob, const std::function<int(sample_list &)> &action) {
	return iob.get("SAMPLES", [&](const char *arguments) {
			arguments = strlen(arguments)==0 ? " " : arguments;
			return fmemopen((void *)arguments, strlen(arguments), "r", [&](FILE *file) {
				sample_list list = {};
				if(1==fscanf(file, "buffer:%d", &list.buffer_key)) {
					while(list.count<MAX_SAMPLES_COUNT
						&& 3==fscanf(file, "%d=>%" PRId64 ",%d", &list.samples[list.count].index, &list.samples[list.count].timestamp, &list.samples[list.count].nb_samples))
						list.count++;
				}
				return action(list);
				});
			});
}

int sample_event(iobus &iob, const std::function<int(int, int, int64_t, int)> &action) {
	int buffer_key, index, samples;
	int64_t pts;
	return iob.get("SAMPLE", [&] {
			return action(buffer_key, index, pts, samples);
			}, 4, "buffer:%d %d=>%" PRId64 ",%d", &buffer_key, &index, &pts, &samples);
}

int audio_event(iobus &iob, const std::function<int(int, int, int64_t, enum AVSampleFormat, int)> &action) {
	int rate, channels, passthrough;
	char layout[128], format[128];
	return iob.get("AUDIO", [&] {
			int res = 0;
			enum AVSampleFormat av_format = av_get_sample_fmt(format);
			if(int64_t av_layout = av_get_channel_layout(layout)) {
				if(AV_SAMPLE_FMT_NONE == av_format)
					res = log_error("Unsupport ffmpeg audio format '%s'", format);
				else
					res = action(rate, channels, av_layout, av_format, passthrough);
			} else
				res = log_error("Unsupport ffmpeg audio layout '%s'", layout);
			return res;
			}, 5, "sample_rate:%d channels:%d layout:%s format:%s passthrough:%d", &rate, &channels, layout, format, &passthrough);
}

int clock_event(iobus &iob, const std::function<int(int64_t, int64_t)> &action) {
	int64_t base, offset;
	return iob.get("CLOCK", [&] {
			return action(base, offset);
			}, 2, "base:%" PRId64 " offset:%" PRId64, &base, &offset);
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

void wait_at_most(int64_t waiting, int64_t least) {
	waiting -= least;
	if (waiting>0)
		usleep(waiting);
}

int64_t analyze_channel_layout(int64_t layout, const char *arg) {
	if(!strlen(arg))
		return layout;
	else if (!strcmp("max2.1", arg)) {
		if(av_get_channel_layout_nb_channels(layout) > 3)
			return AV_CH_LAYOUT_2POINT1;
		else
			return layout;
	} else if (!strcmp("max5.1", arg)) {
		if(av_get_channel_layout_nb_channels(layout) > 6)
			if(AV_CH_LAYOUT_7POINT1_WIDE_BACK == layout)
				return AV_CH_LAYOUT_5POINT1_BACK;
			else
				return AV_CH_LAYOUT_5POINT1;
		else
			return layout;
	} else
		return av_get_channel_layout(arg);
}

int analyze_sample_rate(int rate, const char *arg) {
	int new_rate=0;
	if(!strlen(arg))
		return rate;
	else if(strstr(arg, "max")) {
		sscanf(arg, "max%d", &new_rate);
		if(rate > new_rate)
			return new_rate;
		else
			return rate;
	} else {
		sscanf(arg, "%d", &new_rate);
		return new_rate;
	}
}

enum AVSampleFormat analyze_sample_format(enum AVSampleFormat format, const char *arg) {
	std::regex reg("[^:]+");
	std::string str_arg(arg);
	return std::accumulate(std::sregex_iterator(str_arg.begin(), str_arg.end(), reg), std::sregex_iterator(), format,
			[](enum AVSampleFormat new_format, const std::smatch &matched) {
				std::string command = matched.str();
				if(command == "pack")
					new_format = av_get_packed_sample_fmt(new_format);
				else if(command == "plan")
					new_format = av_get_planar_sample_fmt(new_format);
				else if(command == "int")
					switch(new_format) {
						case AV_SAMPLE_FMT_DBLP:
						case AV_SAMPLE_FMT_FLTP:
							new_format = AV_SAMPLE_FMT_S64P;
							break;
						case AV_SAMPLE_FMT_DBL:
						case AV_SAMPLE_FMT_FLT:
							new_format = AV_SAMPLE_FMT_S64;
							break;
						default:
							break;
					}
				else if(command == "flt32") {
					switch(new_format) {
						case AV_SAMPLE_FMT_DBLP:
							new_format = AV_SAMPLE_FMT_FLTP;
							break;
						case AV_SAMPLE_FMT_DBL:
							new_format = AV_SAMPLE_FMT_FLT;
							break;
						default:
							break;
					}
				}
				else if(command == "flt64") {
					switch(new_format) {
						case AV_SAMPLE_FMT_FLTP:
							new_format = AV_SAMPLE_FMT_DBLP;
							break;
						case AV_SAMPLE_FMT_FLT:
							new_format = AV_SAMPLE_FMT_DBL;
							break;
						default:
							break;
					}
				}
				else if (strstr(command.c_str(), "maxbit")) {
					int maxbit = 32;
					sscanf(command.c_str(), "maxbit%d", &maxbit);
					if(maxbit>=32 && maxbit<64) {
						switch(new_format){
							case AV_SAMPLE_FMT_S64:
								new_format = AV_SAMPLE_FMT_S32;
								break;
							case AV_SAMPLE_FMT_S64P:
								new_format = AV_SAMPLE_FMT_S32P;
								break;
							default:
								break;
						}
					} else if(maxbit>=16 && maxbit<32) {
						switch(new_format){
							case AV_SAMPLE_FMT_S64:
							case AV_SAMPLE_FMT_S32:
								new_format = AV_SAMPLE_FMT_S16;
								break;
							case AV_SAMPLE_FMT_S64P:
							case AV_SAMPLE_FMT_S32P:
								new_format = AV_SAMPLE_FMT_S16P;
								break;
							default:
								break;
						}
					} else if(maxbit>=8 && maxbit<16) {
						switch(new_format){
							case AV_SAMPLE_FMT_S64:
							case AV_SAMPLE_FMT_S32:
							case AV_SAMPLE_FMT_S16:
								new_format = AV_SAMPLE_FMT_U8;
								break;
							case AV_SAMPLE_FMT_S64P:
							case AV_SAMPLE_FMT_S32P:
							case AV_SAMPLE_FMT_S16P:
								new_format = AV_SAMPLE_FMT_U8P;
								break;
							default:
								break;
						}
					}
				} else
					new_format = av_get_sample_fmt(command.c_str());
				return new_format;
				});
}

int layer_event(iobus &iob, const std::function<int(const layer_list &)> &action) {
	return iob.get("LAYER", [&](const char *arguments) {
			arguments = strlen(arguments)==0 ? " " : arguments;
			return fmemopen((void *)arguments, strlen(arguments), "r", [&](FILE *file) {
				layer_list list = {};
				if(5==fscanf(file, "buffer:%d index:%d id:%d width:%d height:%d", &list.buffer_key, &list.index, &list.id, &list.width, &list.height)) {
					while(list.count<MAX_SUB_LAYER_COUNT
						&& 6==fscanf(file, "%d=>%d,%d,%d,%d,%d",
							&list.sub_layers[list.count].offset,
							&list.sub_layers[list.count].x,
							&list.sub_layers[list.count].y,
							&list.sub_layers[list.count].w,
							&list.sub_layers[list.count].h,
							&list.sub_layers[list.count].pitch))
						list.count++;
				}
				return action(list);
				});
			});
}

int reset_event(iobus &iob, const std::function<int(void)> &action) {
	return iob.get("RESET", [&] {
			return action();
			}, 0, "");
}

int nolayer_event(iobus &iob, const std::function<int(int)> &action) {
	int id;
	return iob.get("NOLAYER", [&] { return action(id); }, 1, "id:%d", &id);
}

int control_event(iobus &iob, const std::function<int(int)> &action) {
	int id;
	return iob.get("CONTROL", [&] { return action(id); }, 1, "id:%d", &id);
}

int player_context::start(iobus &iob, const std::function<int(player_context &)> &action) {
	return forward_untill(iob, control_event, [&](int receiver) {
			return msgget([&](int msgid) {
					iob.post("CONTROL id:%d", msgid);
					player_context context(msgid, receiver);
					return action(context);
					});
			});
}

void player_context::process_command() {
	bool paused = false;
	do {
		char buffer[128];
		bool need_reset = false;
		msgrcv(_msgid, [&](const char *command) {
				int64_t value;
				if(!strcmp(command, "p")) {
					paused = true;
				} else if(!strcmp(command, "r")) {
					paused = false;
					_need_sync = true;
				} else if(!strcmp(command, "x")) {
					sprintf(buffer, "%s", command);
					need_reset = true;
				} else if(sscanf(command, "s %" PRId64, &value)==1) {
					sprintf(buffer, "s %" PRId64, value*1000000);
					need_reset = true;
					_need_sync = true;
				} else if(!strcmp(command, "f")) {
					sprintf(buffer, "s %" PRId64, _clock.now()+5000000);
					need_reset = true;
					_need_sync = true;
				} else if(!strcmp(command, "b")) {
					sprintf(buffer, "s %" PRId64, _clock.now()-5000000);
					need_reset = true;
					_need_sync = true;
				} else if(!strcmp(command, "ff")) {
					sprintf(buffer, "s %" PRId64, _clock.now()+15000000);
					need_reset = true;
					_need_sync = true;
				} else if(!strcmp(command, "bb")) {
					sprintf(buffer, "s %" PRId64, _clock.now()-15000000);
					need_reset = true;
					_need_sync = true;
				} else if(sscanf(command, "f %" PRId64, &value)==1) {
					sprintf(buffer, "s %" PRId64, _clock.now()+value*60000000);
					need_reset = true;
					_need_sync = true;
				} else if(sscanf(command, "b %" PRId64, &value)==1) {
					sprintf(buffer, "s %" PRId64, _clock.now()-value*60000000);
					need_reset = true;
					_need_sync = true;
				}
				if(need_reset)
					msgsnd(_receiver, (const char *)buffer, [&]{
							_resetting = true;
							return 0;
							});
				else
					msgsnd(_receiver, command, []{return 0;});
				return 0;
			});
	} while(paused && !usleep(10));
}

enum AVPixelFormat analyze_pixel_format(enum AVPixelFormat format, const char *from, const char *to) {
	enum AVPixelFormat to_format = av_get_pix_fmt(to);
	enum AVPixelFormat cond_format = av_get_pix_fmt(from);
	if(AV_PIX_FMT_NONE != to_format && (AV_PIX_FMT_NONE == cond_format || format == cond_format))
		format = to_format;
	return format;
}

void analyze_size_format(int &w, int &h, const char *arg) {
	int sw, sh;
	if(2==sscanf(arg, "%dx%d", &sw, &sh)) {
		if(w*sh>sw*h) {
			h = sw * h / w;
			w = sw;
		} else {
			w = sh * w / h;
			h = sh;
		}
	}
}

