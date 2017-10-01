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
			if((ret = avformat_find_stream_info(format_context, NULL)) >= 0) {
				res = action(*format_context);
			} else
				res = print_error(ret, app_stderr);
			avformat_close_input(&format_context);
		} else
			res = print_error(ret, app_stderr);
		return res;
}
