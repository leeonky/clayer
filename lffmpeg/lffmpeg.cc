#include <cinttypes>
#include "lffmpeg.h"
#include "stdexd/stdexd.h"

#define MAX_PLAN_NUMBER 16

#undef log_error
#define log_error(format, ...) log_error("liblffmpeg", (format), ## __VA_ARGS__)

#undef log_warning
#define log_warning(format, ...) log_warning("liblffmpeg", (format), ## __VA_ARGS__)

#undef log_errno
#define log_errno(no) log_errno("liblffmpeg", no, av_strerror)

namespace {
	inline void unsupported_operation(const char *fun, enum AVMediaType t) {
		fprintf(stderr, "%s not support [%s] yet\n", fun, av_get_media_type_string(t));
		abort();
	}
}

#define not_support_media_type(t) unsupported_operation(__FUNCTION__, t)

int avformat_open_input(const char *file, const std::function<int(AVFormatContext &)> &action) {
	int res = 0, ret;
	AVFormatContext *format_context = nullptr;
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

const char *avstream_info(const AVStream &stream) {
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
		AVCodecContext *av_codec_context;
		int align;
		int samples_size;
		bool stream_ended = false;
		int64_t previous_pts = 0;
		int64_t previous_duration = 0;
		bool passthrough = false;
		enum AVSampleFormat passthrough_format;
		int64_t passthrough_layout;
		int passthrough_rate, passthrough_channels;
		int passthrough_dts_rate = 0;
	};

	int init_decoding_context(AVStream &stream, AVCodecContext &codec_context, const std::function<int(decoding_context &)> &action) {
		decoding_context context;
		switch(stream.codecpar->codec_type) {
			case AVMEDIA_TYPE_VIDEO:
				context.align = VIDEO_ALIGN;
				break;
			case AVMEDIA_TYPE_AUDIO:
				context.align = 1;
				context.samples_size = stream.codecpar->sample_rate/10;
				break;
			default:
				context.align = 0;
				break;
		}
		context.av_stream = &stream;
		context.av_codec_context = &codec_context;
		return av_new_packet([&](AVPacket &av_packet) -> int {
				context.av_packet = &av_packet;
				return av_new_frame([&](AVFrame &working_av_frame) -> int {
					context.working_av_frame = &working_av_frame;
					working_av_frame.opaque = &context;
					return av_new_frame([&](AVFrame &decoded_av_frame) -> int {
						context.decoded_av_frame = &decoded_av_frame;
						decoded_av_frame.opaque = &context;
						if(AVMEDIA_TYPE_AUDIO == stream.codecpar->codec_type) {
							decoded_av_frame.nb_samples = 0;
							decoded_av_frame.pkt_duration = 0;
							decoded_av_frame.channels = codec_context.channels;
							decoded_av_frame.format = codec_context.sample_fmt;
							int ret = av_samples_alloc(decoded_av_frame.data, decoded_av_frame.linesize, codec_context.channels, context.samples_size, codec_context.sample_fmt, context.align);
							if(ret<0)
								return log_errno(ret);
						}
						return action(context);
						});
					});
				});
	}

	int set_decoder_params(AVCodecContext *context, const codec_params &params) {
		context->thread_count = params.thread_count;
		return 0;
	}
}

const char *avstream_info(const AVCodecContext &codec_context) {
	static __thread char buffer[1024];
	char layout_string_buffer[1024];
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	switch(codec_context.codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			sprintf(buffer, "VIDEO width:%d height:%d format:%s",
					codec_context.width,
					codec_context.height,
					av_get_pix_fmt_name(codec_context.pix_fmt));
			break;
		case AVMEDIA_TYPE_AUDIO:
			if(context->passthrough) {
				av_get_channel_layout_string(layout_string_buffer, sizeof(layout_string_buffer), context->passthrough_channels, context->passthrough_layout);
				sprintf(buffer, "AUDIO sample_rate:%d channels:%d layout:%s format:%s passthrough:%d",
						context->passthrough_rate,
						context->passthrough_channels,
						layout_string_buffer,
						av_get_sample_fmt_name(context->passthrough_format), context->passthrough
						);
			} else {
				av_get_channel_layout_string(layout_string_buffer, sizeof(layout_string_buffer), codec_context.channels, codec_context.channel_layout);
				sprintf(buffer, "AUDIO sample_rate:%d channels:%d layout:%s format:%s passthrough:%d",
						codec_context.sample_rate,
						codec_context.channels,
						layout_string_buffer,
						av_get_sample_fmt_name(codec_context.sample_fmt), context->passthrough
						);
			}
			break;
		default:
			not_support_media_type(codec_context.codec_type);
			break;
	}
	return buffer;
}

int avcodec_open(AVStream &stream, const codec_params &params, const std::function<int(AVCodecContext &)> &action) {
	int res = 0, ret;
	AVCodec *av_codec;
	AVCodecContext *av_codec_context;
	if((av_codec = avcodec_find_decoder(stream.codecpar->codec_id))) {
		if ((av_codec_context = avcodec_alloc_context3(av_codec))) {
			if ((ret=avcodec_parameters_to_context(av_codec_context, stream.codecpar)) >= 0
					&& !set_decoder_params(av_codec_context, params)
					&& (!(ret=avcodec_open2(av_codec_context, av_codec, nullptr)))) {
				res = init_decoding_context(stream, *av_codec_context, [action, av_codec_context](decoding_context &context) -> int {
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

int av_read_frame(AVFormatContext &format_context, AVCodecContext &codec_context, const std::function<void(AVPacket *)> &action) {
	int res;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	while((!(res = av_read_frame(&format_context, context->av_packet)))
			&& context->av_stream->index != context->av_packet->stream_index)
		av_packet_unref(context->av_packet);
	if(res >= 0) {
		action(context->av_packet);
		av_packet_unref(context->av_packet);
	}
	return !(res >= 0);
}

int av_read_and_send_to_avcodec(AVFormatContext &format_context, AVCodecContext &codec_context) {
	int res;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	while((!(res = av_read_frame(&format_context, context->av_packet)))
			&& context->av_stream->index != context->av_packet->stream_index)
		av_packet_unref(context->av_packet);
	if(res >= 0) {
		res = avcodec_send_packet(&codec_context, context->av_packet);
		av_packet_unref(context->av_packet);
	} else {
		avcodec_send_packet(&codec_context, nullptr);
		context->stream_ended = true;
	}
	return res;
}

int avcodec_receive_frame(AVCodecContext &codec_context, const std::function<int(const AVFrame &)> &action) {
	int res;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	AVFrame *wframe = context->working_av_frame;
	if(!(res = avcodec_receive_frame(&codec_context, context->working_av_frame))) {
		switch(codec_context.codec_type) {
			case AVMEDIA_TYPE_VIDEO:
			case AVMEDIA_TYPE_AUDIO:
				wframe->opaque = context;
				res = action(*wframe);
				break;
			default:
				not_support_media_type(codec_context.codec_type);
				return -1;
		}
	}
	return res;
}

int av_get_buffer_size(const AVCodecContext &codec_context) {
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	switch(codec_context.codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			return av_image_get_buffer_size(codec_context.pix_fmt, codec_context.width, codec_context.height, context->align);
		case AVMEDIA_TYPE_AUDIO:
			return context->passthrough ?
				av_samples_get_buffer_size(nullptr, context->passthrough_channels,
						context->samples_size, context->passthrough_format, context->align)
				: av_samples_get_buffer_size(nullptr, codec_context.channels,
						context->samples_size, codec_context.sample_fmt, context->align);
		default:
			not_support_media_type(codec_context.codec_type);
			return -1;
	}
}

int av_copy_frame_to_buffer(const AVFrame &av_frame, void *buf, size_t len) {
	int res=0, ret;
	uint8_t *planar_buffer[MAX_PLAN_NUMBER];
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

namespace {
	inline int64_t guess_duration(const AVFrame &frame) {
		decoding_context *context = static_cast<decoding_context *>(frame.opaque);
		AVCodecContext *codec_context = context->av_codec_context;
		switch(context->av_stream->codecpar->codec_type) {
			case AVMEDIA_TYPE_VIDEO:
				if(codec_context->framerate.num == 0 && codec_context->framerate.den == 1)
					return (int64_t)1000000*125/2997;
				else
					return (int64_t)1000000 * codec_context->framerate.den / codec_context->framerate.num;
			case AVMEDIA_TYPE_AUDIO:
				return (int64_t)frame.nb_samples*1000000/frame.sample_rate;
			default:
				not_support_media_type(context->av_stream->codecpar->codec_type);
				break;
		}
		return -1;
	}
}

int64_t av_frame_pts(const AVFrame &frame) {
	decoding_context *context = static_cast<decoding_context *>(frame.opaque);
	AVStream *stream = context->av_stream; 
	int64_t pts = av_frame_get_best_effort_timestamp(&frame);
	if(AV_NOPTS_VALUE == pts)
		context->previous_pts += context->previous_duration;
	else
		context->previous_pts = av_rescale_q(pts-stream->start_time, stream->time_base, AV_TIME_BASE_Q);
	if(frame.pkt_duration)
		context->previous_duration = av_rescale_q(frame.pkt_duration, stream->time_base, AV_TIME_BASE_Q);
	else
		context->previous_duration = guess_duration(frame);

	return context->previous_pts;
}

int64_t av_frame_pts2(const AVFrame &frame) {
	decoding_context *context = static_cast<decoding_context *>(frame.opaque);
	AVStream *stream = context->av_stream;
	int64_t pts = 1000000;
	if(AV_NOPTS_VALUE != frame.pts)
		pts = pts*(frame.pts-stream->start_time)*stream->time_base.num/stream->time_base.den;
	else
		pts = pts*(av_frame_get_best_effort_timestamp(&frame)-stream->start_time)*stream->time_base.num/stream->time_base.den;
	return pts;
}

const char *av_frame_info(int index, const AVFrame &frame, int buffer_key) {
	decoding_context *context = static_cast<decoding_context *>(frame.opaque);
	static __thread char buffer[1024];
	switch(context->av_stream->codecpar->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			sprintf(buffer, "FRAME buffer:%d %d=>%" PRId64, buffer_key, index, av_frame_pts2(frame));
			break;
		case AVMEDIA_TYPE_AUDIO:
			return av_samples_info(index, av_frame_pts2(frame), frame.nb_samples, buffer_key);
		default:
			not_support_media_type(context->av_stream->codecpar->codec_type);
	}
	return buffer;
}

extern const char *av_samples_info(int index, int64_t pts, int samples, int buffer_key) {
	static __thread char buffer[1024];
	sprintf(buffer, "SAMPLE buffer:%d %d=>%" PRId64 ",%d", buffer_key, index, pts, samples);
	return buffer;
}

int av_image_fill_arrays(int width, int height, enum AVPixelFormat format, const void *buffer, const std::function<int(uint8_t **, int *)> &action) {
	uint8_t* datas[4];
	int lines[4], res;
	int ret = av_image_fill_arrays(datas, lines, (const uint8_t *)buffer, format, width, height, VIDEO_ALIGN);
	if(ret<0)
		res = log_errno(ret);
	else
		res = action(datas, lines);
	return res;
}

int swr_alloc_set_opts_and_init(int64_t in_layout, enum AVSampleFormat in_format, int in_rate, int64_t out_layout, enum AVSampleFormat out_format, int out_rate, const std::function<int(resample_context &)> &action) {
	int res = 0;
	if(SwrContext *swr_context = swr_alloc_set_opts(
				NULL, out_layout, out_format, out_rate,
			       	in_layout, in_format, in_rate, 0, NULL)) {
		if(int ret = swr_init(swr_context))
			res = log_errno(ret);
		else {
			resample_context context;
			context.in_layout = in_layout;
			context.in_format = in_format;
			context.in_rate = in_rate;
			context.in_sample_bytes = av_get_bytes_per_sample(in_format);
			context.in_channels = av_get_channel_layout_nb_channels(in_layout);
			context.out_layout = out_layout;
			context.out_format = out_format;
			context.out_rate = out_rate;
			context.out_sample_bytes = av_get_bytes_per_sample(out_format);
			context.out_channels = av_get_channel_layout_nb_channels(out_layout);
			context.swr_context = swr_context;
			res = action(context);
		}
		swr_free(&swr_context);
	} else
		res = log_error("swr_alloc_set_opts failed");
	return res;
}

int swr_convert(resample_context &context, void *in_buf, size_t size, void *out_buf) {
	int ret = 0;
	uint8_t *outs[MAX_PLAN_NUMBER];
	uint8_t *ins[MAX_PLAN_NUMBER];

	size_t out_size = (size*context.out_rate+context.in_rate-1)/context.in_rate;
	if(av_samples_fill_arrays(outs, nullptr, static_cast<const uint8_t *>(out_buf), context.out_channels, out_size, context.out_format, 1) >= 0) {
		if(av_samples_fill_arrays(ins, nullptr, static_cast<const uint8_t *>(in_buf), context.in_channels, size, context.in_format, 1) >= 0 ) {
			ret = swr_convert(context.swr_context, outs, out_size, const_cast<const uint8_t**>(ins), size);
			if(ret < 0)
				ret = log_error("swr_convert failed");
			else if(ret != static_cast<int>(out_size))
				log_warning("swr_convert expect %d but got %d", out_size, ret);
		}
		else
			ret = log_error("failed to fill input samples to arrays");
	} else
		ret = log_error("failed to fill output samples to arrays");
	return ret;
}

size_t resample_context::resample_size() const {
	return av_samples_get_buffer_size(nullptr, out_channels,
			out_rate/10, out_format, 1);
}

int av_seek_frame(AVFormatContext &format_context, int64_t time, const std::function<int(void)> &action) {
	int res = avformat_seek_file(&format_context, -1,
			time-100000, time, time+100000, AVSEEK_FLAG_BACKWARD);
	if(!res)
		res = action();
	return res;
}

int sws_getContext(int in_w, int in_h, enum AVPixelFormat in_format,
		int out_w, int out_h, enum AVPixelFormat out_format,
		int flag, const std::function<int(scale_context &)> &action) {
	scale_context context{in_w, in_h, in_format, out_w, out_h, out_format, flag,
		sws_getContext(in_w, in_h, in_format, out_w, out_h, out_format, flag, nullptr, nullptr, nullptr)};
	int res = 0;
	if(context.sws_context) {
		res = action(context);
		sws_freeContext(context.sws_context);
	} else
		res = log_error("failed to sws_getContext");
	return res;
}

int sws_scale(scale_context &context, const void *in_buf, void *out_buf) {
	return av_image_fill_arrays(context.out_w, context.out_h, context.out_format, out_buf, [&](uint8_t **out_data, int *out_lines) {
			return av_image_fill_arrays(context.in_w, context.in_h, context.in_format, in_buf, [&](uint8_t **in_data, int *in_lines) {
					sws_scale(context.sws_context, in_data, in_lines, 0, context.in_h, out_data, out_lines);
					return 0;
					});
			});
}

size_t scale_context::rescale_size() const {
	return av_image_get_buffer_size(out_format, out_w, out_h, VIDEO_ALIGN);
}

bool passthrough_process(AVCodecContext &codec_context) {
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	switch(codec_context.codec_id) {
		case AV_CODEC_ID_EAC3:
			context->passthrough = true;
			context->passthrough_format = AV_SAMPLE_FMT_S16;
			context->passthrough_rate = codec_context.sample_rate;
			context->passthrough_channels = 8;
			context->passthrough_layout = AV_CH_LAYOUT_7POINT1;
			break;
		case AV_CODEC_ID_AC3:
			context->passthrough = true;
			context->passthrough_format = AV_SAMPLE_FMT_S16;
			context->passthrough_rate = codec_context.sample_rate;
			context->passthrough_channels = 2;
			context->passthrough_layout = AV_CH_LAYOUT_STEREO;
			break;
		case AV_CODEC_ID_TRUEHD:
			context->passthrough = true;
			context->passthrough_rate = 192000;
			context->passthrough_format = AV_SAMPLE_FMT_S16;
			context->passthrough_channels = 8;
			context->passthrough_layout = AV_CH_LAYOUT_7POINT1;
			break;
		case AV_CODEC_ID_DTS:
			context->passthrough = true;
			context->passthrough_format = AV_SAMPLE_FMT_S16;
			if(FF_PROFILE_DTS_HD_MA==codec_context.profile || FF_PROFILE_DTS_HD_HRA==codec_context.profile) {
				context->passthrough_rate = 192000;
				context->passthrough_channels = 8;
				context->passthrough_layout = AV_CH_LAYOUT_7POINT1;
				context->passthrough_dts_rate = 768000;
			} else {
				context->passthrough_rate = 48000;
				context->passthrough_channels = 2;
				context->passthrough_layout = AV_CH_LAYOUT_STEREO;
			}
			break;
		default:
			context->passthrough = false;
			break;
	}
	return context->passthrough;
}

namespace {
	struct write_packet_arg {
		decoding_context *context;
		const std::function<void(void *, int, int)> *call_back;
	};

	int write_packet(void *opaque, uint8_t *buf, int size) {
		write_packet_arg *arg = (write_packet_arg *)opaque;
		int sample_byes = av_get_bytes_per_sample(arg->context->passthrough_format)*arg->context->passthrough_channels;
		int frame_size = size/sample_byes;
		size = frame_size*sample_byes;
		(*(arg->call_back))(buf, frame_size, size);
		return size;
	}
}

int avformat_alloc_passthrough_context(AVCodecContext &codec_context, const std::function<int(AVFormatContext &)> &action, const std::function<void(void *, int, int)> &buffer_handler) {
	int res = 0, ret;
	decoding_context *context = static_cast<decoding_context *>(codec_context.opaque);
	AVFormatContext *format_context = NULL;
	if((ret = avformat_alloc_output_context2(&format_context, NULL, "spdif", "")) >= 0) {
		int len = av_get_buffer_size(codec_context);
		if(void *buffer = av_malloc(len)) {
			if(AVStream *stream = avformat_new_stream(format_context, codec_context.codec)) {
				if((ret = avcodec_parameters_copy(stream->codecpar, context->av_stream->codecpar))>=0) {
					write_packet_arg args{context, &buffer_handler};
					if((format_context->pb = avio_alloc_context((uint8_t *)buffer, len, 1, &args, NULL, write_packet, NULL))) {
						if(context->passthrough_dts_rate)
							av_opt_set_int(format_context->priv_data, "dtshd_rate", context->passthrough_dts_rate, 0);

						res = action(*format_context);

						avio_context_free(&format_context->pb);
					} else
						res = log_error("avio_alloc_context failed");
				} else
					res = log_errno(ret);
			} else
				res = log_error("create AVStream for output error");
			av_free(buffer);
		} else
			res = log_error("alloc buffer failed");
		avformat_free_context(format_context);
	} else
		res = log_errno(ret);
	return res;
}

int avformat_write_header(AVFormatContext &out_format) {
	int r = avformat_write_header(&out_format, NULL);
	if (r < 0)
		log_errno(r);
	return r;
}

int av_write_frame(AVFormatContext &out_format, AVPacket *packet) {
	// set cur_dts to zero avoid warning message when back seek stream
	out_format.streams[0]->cur_dts = 0;
	int r = av_write_frame(&out_format, packet);
	if (r < 0)
		log_errno(r);
	return r;
}


