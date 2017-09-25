#ifndef FFMPEGPP_FFMPEGPP_H
#define FFMPEGPP_FFMPEGPP_H
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

class ffmpeg_stream {
	public:
		AVFormatContext *format_context;
};

class ffmpeg {
	public:
		static int open_stream(const char *, enum AVMediaType, int, std::function<int(ffmpeg_stream &)>);
};

#endif
