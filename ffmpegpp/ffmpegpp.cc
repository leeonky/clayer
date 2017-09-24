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
}

int ffmpeg::open_stream(const char *file, enum AVMediaType type, int index, std::function<int(ffmpeg_stream &)> action) {
	int res = 0;
	if(action) {
		av_register_all();
		res = avformat_open_input(file,
				[action](AVFormatContext *format_context)-> int {
				return avformat_find_stream_info(format_context,
					[action](AVFormatContext *format_context)-> int {
					ffmpeg_stream stream;
					return action(stream);
					});
				});
	}
	return res;
}
