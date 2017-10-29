#include "lffmpeg.h"
#include "stdexd/stdexd.h"

#undef log_error
#define log_error(format, ...) log_error("liblffmpeg", (format), ## __VA_ARGS__)

#undef log_errno
#define log_errno(no) log_errno("liblffmpeg", no, av_strerror)

namespace {
	static inline void unsupported_operation(const char *fun, enum AVMediaType t) {
		fprintf(stderr, "%s not support [%s] yet\n", fun, av_get_media_type_string(t));
		abort();
	}
}

#define not_support_media_type(t) unsupported_operation(__FUNCTION__, t)

int avformat_open_input(const char *file, const std::function<int(AVFormatContext &)> &action) {
	int res = 0, ret;
	AVFormatContext *format_context;
	av_register_all();
	if (!(ret = avformat_open_input(&format_context, file, nullptr, nullptr))) {
		res = action(*format_context);
		avformat_close_input(&format_context);
	} else
		res = log_errno(ret);
	return res;
}

int avformat_find_stream(AVFormatContext &av_format_context, enum AVMediaType type, int track, const std::function<int(AVStream &)> &action) {
	int res, ret;
	if(-1 == track)
		track = 0;
	if((ret = avformat_find_stream_info(&av_format_context, nullptr)) >= 0) {
		int matched = 0;
		for (size_t i=0; i<av_format_context.nb_streams; ++i)
			if(av_format_context.streams[i]->codecpar->codec_type == type && matched++ == track)
				return action(*av_format_context.streams[i]);
		res = log_error("%s stream %d doesn't exist", av_get_media_type_string(type), track);
	} else
		res = log_errno(ret);
	return res;
}

char *avstream_info(const AVStream &stream) {
	static __thread char buffer[1024];
	char layout_string_buffer[1024];
	AVCodecParameters *parameters = stream.codecpar;
	switch(parameters->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			sprintf(buffer, "VIDEO width:%d height:%d format:%s",
					parameters->width,
					parameters->height,
					av_get_pix_fmt_name((enum AVPixelFormat)parameters->format));
			break;
		case AVMEDIA_TYPE_AUDIO:
			av_get_channel_layout_string(layout_string_buffer, sizeof(layout_string_buffer), parameters->channels, parameters->channel_layout);
			sprintf(buffer, "AUDIO sample_rate:%d channels:%d layout:%s format:%s",
					parameters->sample_rate,
					parameters->channels,
					layout_string_buffer,
					av_get_sample_fmt_name((enum AVSampleFormat)parameters->format));
			break;
		default:
			not_support_media_type(parameters->codec_type);
			break;
	}
	return buffer;
}

int av_new_packet(const std::function<int(AVPacket &)> &action) {
	int res;
	AVPacket av_packet;
	av_init_packet(&av_packet);
	res = action(av_packet);
	av_packet_unref(&av_packet);
	return res;
}

int av_new_frame(const std::function<int(AVFrame &)> &action) {
	int res;
	AVFrame *av_frame = av_frame_alloc();
	if(av_frame) {
		res = action(*av_frame);
		av_frame_free(&av_frame);
	} else
		res = log_error("failed to alloc AVFrame");
	return res;
}

namespace {
	struct decoding_context {
		AVPacket *av_packet;
		AVFrame *working_av_frame, *decoded_av_frame;
		AVStream *av_stream;
		int align;
		int samples_size;
	};

	int init_decoding_context(AVStream &stream, const std::function<int(decoding_context &)> &action) {
		decoding_context context;
		switch(stream.codecpar->codec_type) {
			case AVMEDIA_TYPE_VIDEO:
				context.align = 64;
				break;
			case AVMEDIA_TYPE_AUDIO:
				context.align = 1;
				break;
			default:
				context.align = 0;
				break;
		}
		context.av_stream = &stream;
		return av_new_packet([&](AVPacket &av_packet) -> int {
				context.av_packet = &av_packet;
				return av_new_frame([&](AVFrame &working_av_frame) -> int {
					context.working_av_frame = &working_av_frame;
					working_av_frame.opaque = &context;
					return av_new_frame([&](AVFrame &decoded_av_frame) -> int {
						context.decoded_av_frame = &decoded_av_frame;
						decoded_av_frame.opaque = &context;
						return action(context);
						});
					});
				});
	}
}

int avcodec_open(AVStream &stream, const std::function<int(AVCodecContext &)> &action) {
	int res = 0, ret;
	AVCodec *av_codec;
	AVCodecContext *av_codec_context;
	if((av_codec = avcodec_find_decoder(stream.codecpar->codec_id))) {
		if ((av_codec_context = avcodec_alloc_context3(av_codec))) {
			if ((ret=avcodec_parameters_to_context(av_codec_context, stream.codecpar)) >= 0
					&& (!(ret=avcodec_open2(av_codec_context, av_codec, nullptr)))) {
				res = init_decoding_context(stream, [action, av_codec_context](decoding_context &context) -> int {
						av_codec_context->opaque = &context;
						return action(*av_codec_context);
						});
				avcodec_close(av_codec_context);
			} else
				res = log_errno(ret);
			avcodec_free_context(&av_codec_context);
		} else
			res = log_error("failed to alloc AVCodecContext");
	} else
		res = log_error("failed to find decoder");
	return res;
}

int av_read_and_send_to_avcodec(AVFormatContext &format_context, AVCodecContext &codec_context) {
	int res;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	while((!(res = av_read_frame(&format_context, context->av_packet)))
			&& context->av_stream->index != context->av_packet->stream_index)
		;
	if(res >= 0)
		res = avcodec_send_packet(&codec_context, context->av_packet);
	else
		avcodec_send_packet(&codec_context, nullptr);
	return res;
}

int avcodec_receive_frame(AVCodecContext &codec_context, const std::function<int(const AVFrame &)> &action) {
	int res;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	if(!(res = avcodec_receive_frame(&codec_context, context->working_av_frame))) {
		res = action(*context->working_av_frame);
	}
	return res;
}

int av_get_buffer_size(const AVCodecContext &codec_context) {
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	switch(codec_context.codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			return av_image_get_buffer_size(codec_context.pix_fmt, codec_context.width, codec_context.height, context->align);
		case AVMEDIA_TYPE_AUDIO:
			return av_samples_get_buffer_size(nullptr, codec_context.channels,
					context->samples_size, codec_context.sample_fmt, context->align);
		default:
			not_support_media_type(codec_context.codec_type);
			return -1;
	}
}

int av_copy_frame_to_buffer(const AVFrame &av_frame, void *buf, size_t len) {
	int res=0, ret;
	uint8_t *planar_buffer[12];
	decoding_context *context = static_cast<decoding_context *>(av_frame.opaque);
	switch(context->av_stream->codecpar->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			if((ret=av_image_copy_to_buffer(static_cast<uint8_t *>(buf),
							len, av_frame.data,
							av_frame.linesize,
							static_cast<AVPixelFormat>(av_frame.format),
							av_frame.width, av_frame.height, context->align)) < 0)
				res = log_errno(ret);
			break;
		case AVMEDIA_TYPE_AUDIO:
			if((ret=av_samples_fill_arrays(planar_buffer, nullptr,
							static_cast<const uint8_t *>(buf),
							av_frame.channels, av_frame.nb_samples,
							static_cast<AVSampleFormat>(av_frame.format), context->align)) >= 0)
				res = av_samples_copy(planar_buffer, av_frame.data,
						0, 0, av_frame.nb_samples, av_frame.channels,
						static_cast<AVSampleFormat>(av_frame.format));
			else
				res = log_errno(ret);
			break;
		default:
			not_support_media_type(context->av_stream->codecpar->codec_type);
			return -1;
	}
	return res;
}

