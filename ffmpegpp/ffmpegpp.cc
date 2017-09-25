#include <cstdio>
#include <functional>
#include "stdexd/stdexd.h"
#include "ffmpegpp.h"

namespace {
	int print_error(int no, FILE *stderr) {
		char buffer[1024];
		av_strerror(no, buffer, sizeof(buffer));
		fprintf(stderr, "Error[libwrpffp]: %s\n", buffer);
		print_stack(stderr);
		return -1;
	}

	int avformat_open_input(const char *file, std::function<int(AVFormatContext *)> action) {
		int res = 0, ret;
		AVFormatContext *format_context;
		if (!(ret = avformat_open_input(&format_context, file, NULL, NULL))) {
			res = action(format_context);
			avformat_close_input(&format_context);
		} else
			res = print_error(ret, app_stderr);
		return res;
	}

	int avformat_find_stream_info(AVFormatContext *format_context, std::function<int(AVFormatContext *)> action) {
		int res = 0, ret;
		if((ret = avformat_find_stream_info(format_context, NULL)) >= 0) {
			res = action(format_context);
		} else
			res = print_error(ret, app_stderr);
		return res;
	}

	int select_av_stream(AVFormatContext *format_context, enum AVMediaType type, int track, std::function<int(AVStream *)> action) {
		size_t i;
		int matched = 0;
		for (i=0; i<format_context->nb_streams; ++i) {
			if(format_context->streams[i]->codecpar->codec_type == type && matched++ == track) {
				return action(format_context->streams[i]);
			}
		}
		fprintf(app_stderr, "Error[libwrpffp]: %s stream %d doesn't exist\n", av_get_media_type_string(type), track);
		return -1;
	}

	int av_init_packet(AVPacket &av_packet, std::function<int()> action) {
		int res;
		av_init_packet(&av_packet);
		res = action();
		av_packet_unref(&av_packet);
		return res;
	}
}

int ffmpeg::open_stream(const char *file, enum AVMediaType type, int index, std::function<int(ffmpeg_stream &)> action) {
	int res = 0;
	if(action) {
		av_register_all();
		res = avformat_open_input(file,
				[type, index, action](AVFormatContext *format_context) -> int {
				return avformat_find_stream_info(format_context,
					[type, index, action](AVFormatContext *format_context) -> int {
					ffmpeg_stream stream;
					stream.av_format_context = format_context;
					return select_av_stream(format_context, type, (index<0 ? 0 : index), [action, &stream](AVStream *av_stream) -> int {
						stream.av_stream = av_stream;
						return av_init_packet(stream.av_packet, [action, &stream]() -> int {
							return action(stream);
							});
						});
					});
				});
	}
	return res;
}
