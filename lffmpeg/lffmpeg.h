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

extern char *avstream_info(const AVStream &);

extern int av_new_packet(std::function<int(AVPacket &)>);

extern int av_new_frame(std::function<int(AVFrame &)>);

extern int avcodec_open(AVStream &, std::function<int(AVCodecContext &)>);

extern int av_read_and_send_to_avcodec(AVFormatContext &, AVCodecContext &);

#endif

