#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"

namespace {
	int track_index = -1;
	enum AVMediaType track_type = AVMEDIA_TYPE_VIDEO;
	int buffer_count = 16;
	int buffer_key = 0;
	int msgid = -1;
	const char *file_name;
	iobus iob(app_stdin, app_stdout, app_stderr);

	void process_args(int argc, char **argv) {
		file_name = command_argument()
			.require_full_argument("count", 'c', [&](const char *arg){
					sscanf(arg, "%d", &buffer_count);
					})
		.require_full_argument("key", 'k', [&](const char *arg){
				sscanf(arg, "%d", &buffer_key);
				})
		.require_full_argument("video", 'v', [&](const char *arg){
				sscanf(arg, "%d", &track_index);
				track_type = AVMEDIA_TYPE_VIDEO;
				})
		.require_full_argument("audio", 'a', [&](const char *arg){
				sscanf(arg, "%d", &track_index);
				track_type = AVMEDIA_TYPE_AUDIO;
				}).parse(argc, argv);

		if(!file_name) {
			fprintf(app_stderr, "Error[decoder]: require media file\n");
			exit(-1);
		}
	}

	inline std::function<int(const AVFrame &)> copy_frame_to_buffer_and_post(circular_shm &buffer) {
		return [&](const AVFrame &frame){
			if(!av_copy_frame_to_buffer(frame, buffer.allocate(), buffer.element_size))
				iob.post(av_frame_info(buffer.index, frame, buffer_key));
			return 0;
		};
	}

	inline std::function<int(circular_shm &)> decoding_loop(iobus &iob, AVFormatContext &format_context, AVCodecContext &codec_context) {
		return [&](circular_shm &buffer){
			iob.post(buffer.serialize_to_string(buffer_key));
			auto copy_and_post = copy_frame_to_buffer_and_post(buffer);
			while(!av_read_and_send_to_avcodec(format_context, codec_context))
				avcodec_receive_frame(codec_context, copy_and_post);
			while(!avcodec_receive_frame(codec_context, copy_and_post))
				;
			return 0;
		};
	}

	inline std::function<int(AVStream &)> open_and_decoding(iobus &iob, AVFormatContext &format_context) {
		return [&](AVStream &stream){
			return avcodec_open(stream, [&](AVCodecContext &codec_context){
					iob.post(avstream_info(stream));
					return circular_shm::create(av_get_buffer_size(codec_context),
							buffer_count, decoding_loop(iob, format_context, codec_context));
					});
		};
	}
}

int main(int argc, char **argv) {
	process_args(argc, argv);
	return msgget([&](int id) {
			msgid = id;
			iob.post("CONTROL id:%d", msgid);
			return avformat_open_input(file_name, [](AVFormatContext &format_context){
					return avformat_find_stream(format_context, track_type, track_index, open_and_decoding(iob, format_context));
					});
			});
}

