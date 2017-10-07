#include "lffmpeg.h"
#include "stdexd/stdexd.h"

namespace {
	int print_error(int no) {
		char buffer[1024];
		av_strerror(no, buffer, sizeof(buffer));
		fprintf(app_stderr, "Error[liblffmpeg]: %s\n", buffer);
		print_stack(app_stderr);
		return -1;
	}

	static inline void unsupported_operation(const char *fun, enum AVMediaType t) {
		fprintf(stderr, "%s not support [%s] yet\n", fun, av_get_media_type_string(t));
		abort();
	}
}

#define not_support_media_type(t) unsupported_operation(__FUNCTION__, t)

int avformat_open_input(const char *file, std::function<int(AVFormatContext &)> action) {
	int res = 0, ret;
	AVFormatContext *format_context;
	av_register_all();
	if (!(ret = avformat_open_input(&format_context, file, NULL, NULL))) {
		res = action(*format_context);
		avformat_close_input(&format_context);
	} else
		res = print_error(ret);
	return res;
}

int avformat_find_stream(AVFormatContext &av_format_context, enum AVMediaType type, int track, std::function<int(AVStream &)> action) {
	int res, ret;
	if(-1 == track)
		track = 0;
	if((ret = avformat_find_stream_info(&av_format_context, NULL)) >= 0) {
		int matched = 0;
		for (size_t i=0; i<av_format_context.nb_streams; ++i)
			if(av_format_context.streams[i]->codecpar->codec_type == type && matched++ == track)
				return action(*av_format_context.streams[i]);
		fprintf(app_stderr, "Error[liblffmpeg]: %s stream %d doesn't exist\n", av_get_media_type_string(type), track);
		res = -1;
	} else
		res = print_error(ret);
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

int avcodec_open(AVStream &stream, std::function<int(AVCodecContext &)> action) {
	int res = 0, ret;
	AVCodec *av_codec;
	AVCodecContext *av_codec_context;
	if((av_codec = avcodec_find_decoder(stream.codecpar->codec_id))) {
		if ((av_codec_context = avcodec_alloc_context3(av_codec))) {
			if ((ret=avcodec_parameters_to_context(av_codec_context, stream.codecpar)) >= 0
					&& (!(ret=avcodec_open2(av_codec_context, av_codec, NULL)))) {
				res = action(*av_codec_context);
				avcodec_close(av_codec_context);
			} else {
				res = print_error(ret);
			}
			avcodec_free_context(&av_codec_context);
		} else {
			res = -1;
			fprintf(app_stderr, "Error[liblffmpeg]: failed to alloc AVCodecContext\n");
		}
	} else {
		res = -1;
		fprintf(app_stderr, "Error[liblffmpeg]: failed to find decoder\n");
	}
	return res;
}

