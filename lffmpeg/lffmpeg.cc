#include "lffmpeg.h"
#include "stdexd/stdexd.h"

namespace {
	int print_error(int no, FILE *stderr) {
		char buffer[1024];
		av_strerror(no, buffer, sizeof(buffer));
		fprintf(stderr, "Error[liblffmpeg]: %s\n", buffer);
		print_stack(stderr);
		return -1;
	}
}

int avformat_open_input(const char *file, std::function<int(AVFormatContext &)> action) {
	int res = 0, ret;
	AVFormatContext *format_context;
	av_register_all();
	if (!(ret = avformat_open_input(&format_context, file, NULL, NULL))) {
		res = action(*format_context);
		avformat_close_input(&format_context);
	} else
		res = print_error(ret, app_stderr);
	return res;
}

int avformat_find_stream(AVFormatContext &av_format_context, enum AVMediaType type, int track, std::function<int(AVStream &)> action) {
	int res = 0, matched = 0, ret;
	size_t i;
	if(-1 == track)
		track = 0;
	if((ret = avformat_find_stream_info(&av_format_context, NULL)) >= 0) {
		for (i=0; i<av_format_context.nb_streams; ++i)
			if(av_format_context.streams[i]->codecpar->codec_type == type && matched++ == track)
				return action(*av_format_context.streams[i]);
	} else
		res = print_error(ret, app_stderr);
	return res;
}
