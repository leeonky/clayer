#ifndef LFFMPEG_LFFMPEG_H
#define LFFMPEG_LFFMPEG_H
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

extern int avformat_open_input(const char *, std::function<int(AVFormatContext &)>);

extern int avformat_find_stream(AVFormatContext &, enum AVMediaType, int, std::function<int(AVStream &)>);

#endif

