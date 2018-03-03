#ifndef LFFMPEG_LFFMPEG_H
#define LFFMPEG_LFFMPEG_H
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#define VIDEO_ALIGN 64

extern int avformat_open_input(const char *, const std::function<int(AVFormatContext &)> &);

extern int avformat_find_stream(AVFormatContext &, enum AVMediaType, int, const std::function<int(AVStream &)> &);

extern const char *avstream_info(const AVStream &);

extern int av_new_packet(const std::function<int(AVPacket &)> &);

extern int av_new_frame(const std::function<int(AVFrame &)> &);

extern int avcodec_open(AVStream &, const std::function<int(AVCodecContext &)> &);

extern int av_read_and_send_to_avcodec(AVFormatContext &, AVCodecContext &);

extern int avcodec_receive_frame(AVCodecContext &, const std::function<int(const AVFrame &)> &);

extern int av_get_buffer_size(const AVCodecContext &);

extern int av_copy_frame_to_buffer(const AVFrame &, void *, size_t);

extern int64_t av_frame_pts(const AVFrame &);

extern const char *av_frame_info(int, const AVFrame &, int buffer_key=0);

extern int av_image_fill_arrays(int, int, enum AVPixelFormat, const void *, const std::function<int(uint8_t **, int *)> &);

struct resample_context {
	int64_t in_layout, out_layout;
       	enum AVSampleFormat in_format, out_format;
       	int in_rate, out_rate;
	int in_sample_bytes, out_sample_bytes;
       	int in_channels, out_channels;
	SwrContext *swr_context;

	size_t resample_size(size_t size) const {
		size_t out_unit_size = out_sample_bytes*out_channels*out_rate;
		size_t in_unit_size = in_sample_bytes*in_channels*in_rate;
		return (size*out_unit_size+in_unit_size-1)/in_unit_size;
	}
};

extern int swr_alloc_set_opts_and_init(int64_t, enum AVSampleFormat, int, int64_t, enum AVSampleFormat, int, const std::function<int(resample_context &)> &);

extern int swr_convert(resample_context &, void *, size_t, void *);

#endif

