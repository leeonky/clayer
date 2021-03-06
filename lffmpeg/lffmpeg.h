#ifndef LFFMPEG_LFFMPEG_H
#define LFFMPEG_LFFMPEG_H
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#define VIDEO_ALIGN 64

extern int avformat_open_input(const char *, const std::function<int(AVFormatContext &)> &);

extern int avformat_find_stream(AVFormatContext &, enum AVMediaType, int, const std::function<int(AVStream &)> &);

extern const char *avstream_info(const AVStream &);

extern const char *avstream_info(const AVCodecContext &);

extern int av_new_packet(const std::function<int(AVPacket &)> &);

extern int av_new_frame(const std::function<int(AVFrame &)> &);

struct codec_params {
	int thread_count = 1;
};

extern int avcodec_open(AVStream &, const codec_params &, const std::function<int(AVCodecContext &)> &);

extern int av_read_frame(AVFormatContext &, AVCodecContext &, const std::function<void(AVPacket *)> &);

extern int av_read_and_send_to_avcodec(AVFormatContext &, AVCodecContext &);

extern int avcodec_receive_frame(AVCodecContext &, const std::function<int(const AVFrame &)> &);

extern int av_get_buffer_size(const AVCodecContext &);

extern int av_copy_frame_to_buffer(const AVFrame &, void *, size_t);

extern int64_t av_frame_pts(const AVFrame &);

extern int64_t av_frame_pts2(const AVFrame &);

extern const char *av_frame_info(int, const AVFrame &, int buffer_key=0);

extern const char *av_samples_info(int, int64_t, int, int buffer_key=0);

extern int av_image_fill_arrays(int, int, enum AVPixelFormat, const void *, const std::function<int(uint8_t **, int *)> &);

struct resample_context {
	int64_t in_layout, out_layout;
       	enum AVSampleFormat in_format, out_format;
       	int in_rate, out_rate;
	int in_sample_bytes, out_sample_bytes;
       	int in_channels, out_channels;
	SwrContext *swr_context;

	size_t resample_size() const;
};

extern int swr_alloc_set_opts_and_init(int64_t, enum AVSampleFormat, int, int64_t, enum AVSampleFormat, int, const std::function<int(resample_context &)> &);

extern int swr_convert(resample_context &, void *, size_t, void *);

extern int av_seek_frame(AVFormatContext &, int64_t, const std::function<int(void)> &);

struct scale_context {
	int in_w, in_h;
	enum AVPixelFormat in_format;
	int out_w, out_h;
	enum AVPixelFormat out_format;
	int flag;
	SwsContext *sws_context;

	size_t rescale_size() const;
};

extern int sws_getContext(int, int, enum AVPixelFormat, int, int, enum AVPixelFormat, int, const std::function<int(scale_context &)> &);

extern int sws_scale(scale_context &, const void *, void *);

extern bool passthrough_process(AVCodecContext &);

extern int avformat_alloc_passthrough_context(AVCodecContext &, const std::function<int(AVFormatContext &)> &, const std::function<void(void *, int, int)> &);

extern int avformat_write_header(AVFormatContext &);
extern int av_write_frame(AVFormatContext &, AVPacket *);

#endif

