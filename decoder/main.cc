#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"

namespace {
	bool passthrough = false;
	int track_index = -1;
	enum AVMediaType track_type = AVMEDIA_TYPE_VIDEO;
	int buffer_count = 16;
	int buffer_key = 0;
	int msgid = -1;
	const char *file_name;
	codec_params params;
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
		.require_full_argument("thread", 't', [&](const char *arg){
				sscanf(arg, "%d", &params.thread_count);
				})
		.require_full_argument("audio", 'a', [&](const char *arg){
				sscanf(arg, "%d", &track_index);
				track_type = AVMEDIA_TYPE_AUDIO;
				}).require_option("pass", 'p', [&](const char *){
					passthrough = true;
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
			bool running = true;
			while(running && !av_read_and_send_to_avcodec(format_context, codec_context)) {
				avcodec_receive_frame(codec_context, copy_and_post);
				msgrcv(msgid, [&](const char *command) {
						int64_t value;
						if(1==sscanf(command, "s %" PRId64, &value)) {
							avcodec_flush_buffers(&codec_context);
							av_seek_frame(format_context, value, [&]() {
									iob.post("RESET");
									return 0;
								});
						} else if(!strcmp(command, "x"))
							running = false;
						return 0;
					});
			}
			while(!avcodec_receive_frame(codec_context, copy_and_post))
				;
			return 0;
		};
	}

	inline std::function<int(circular_shm &)> passthrough_loop(iobus &iob, AVFormatContext &format_context, AVCodecContext &codec_context) {
		return [&](circular_shm &buffer){
			iob.post(buffer.serialize_to_string(buffer_key));
			return avformat_alloc_passthrough_context(codec_context, [&](AVFormatContext &out_format) {
					bool running = true;
					//avformat_write_header(&out_format, NULL);
					//out_format.oformat->write_header(&out_format);
					while(running && !av_read_frame(format_context, codec_context, [&](AVPacket *packet){
								//av_write_frame(&out_format, packet);
								//out_format.oformat->write_packet(&out_format, packet);
								})) {
					}
					return 0;
					}, [&](void *buf, int size, int samples){
						//memcpy(buffer.allocate(), buf, size);
						//iob.post(av_samples_info(buffer.index, 0, samples, buffer_key));
					});
		};
	}

	inline std::function<int(AVStream &)> open_and_decoding(iobus &iob, AVFormatContext &format_context) {
		return [&](AVStream &stream){
			return avcodec_open(stream, params, [&](AVCodecContext &codec_context){
					passthrough = passthrough && passthrough_process(codec_context);
					iob.post(avstream_info(codec_context));
					return circular_shm::create(av_get_buffer_size(codec_context),
							buffer_count,
							(passthrough ?
							passthrough_loop(iob, format_context, codec_context)
							: decoding_loop(iob, format_context, codec_context)));
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

