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
		AVFormatContext *av_format_context;
		AVStream *av_stream;
		AVPacket av_packet;
};

class ffmpeg {
	public:
		static int open_stream(const char *, enum AVMediaType, int, std::function<int(ffmpeg_stream &)>);
};

#endif
