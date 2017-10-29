#ifndef LFFMPEG_LFFMPEG_H
#define LFFMPEG_LFFMPEG_H
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

extern int avformat_open_input(const char *, const std::function<int(AVFormatContext &)> &);

extern int avformat_find_stream(AVFormatContext &, enum AVMediaType, int, const std::function<int(AVStream &)> &);

extern char *avstream_info(const AVStream &);

extern int av_new_packet(const std::function<int(AVPacket &)> &);

extern int av_new_frame(const std::function<int(AVFrame &)> &);

extern int avcodec_open(AVStream &, const std::function<int(AVCodecContext &)> &);

extern int av_read_and_send_to_avcodec(AVFormatContext &, AVCodecContext &);

extern int avcodec_receive_frame(AVCodecContext &, const std::function<int(const AVFrame &)> &);

extern int av_get_buffer_size(const AVCodecContext &);

extern int av_copy_frame_to_buffer(const AVFrame &, void *, size_t);

extern int64_t av_frame_pts(const AVFrame &);

#endif

