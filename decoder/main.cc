#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"

int main(int argc, char **argv) {
	int track_index = -1;
	enum AVMediaType track_type = AVMEDIA_TYPE_VIDEO;
	int buffer_count = 16;
	const char *file_name = command_argument()
		.require_full_argument("count", 'c', [&](const char *arg){
				sscanf(arg, "%d", &buffer_count);
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
		return -1;
	}

	return avformat_open_input(file_name,
			[=](AVFormatContext &format_context){
			return avformat_find_stream(format_context, track_type, track_index,
				[&](AVStream &stream){
				iobus iob(app_stdin, app_stdout, app_stderr);
				iob.post(avstream_info(stream));
				return avcodec_open(stream,
					[&](AVCodecContext &codec_context){
					return circular_shm::create(av_get_buffer_size(codec_context), buffer_count,
						[&](circular_shm &buffer){
						iob.post(buffer.serialize_to_string());
						auto frame_decoded = [&](const AVFrame &frame){
						if(!av_copy_frame_to_buffer(frame, buffer.allocate(), buffer.element_size))
							iob.post(av_frame_info(buffer.index, frame));
						return 0;
						};

						while(!av_read_and_send_to_avcodec(format_context, codec_context))
							avcodec_receive_frame(codec_context, frame_decoded);
						while(!avcodec_receive_frame(codec_context, frame_decoded))
						;
						return 0;
						});
					});
					});
				});
}

