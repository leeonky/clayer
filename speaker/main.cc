#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "lport_audio/lport_audio.h"
#include "media/media.h"

namespace {
	circular_shm *shms[MAX_LAYER_COUNT];

	int play_with_portaudio(iobus &iob, int device, player_context &context) {
		return forward_untill(iob, audio_event, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format){
				return Pa_Init_OpenOutputStream(device, sample_rate, channels, AVSampleFormat_to_PortAudio(format), [&](PaStream *stream){
						long buffer_len = Pa_GetStreamWriteAvailable(stream);
						return main_reducer(iob, shms, sample_event, [&](int buffer_key, int index, int64_t pts, int samples) {
								context.process_command();
								if(samples) {
									int64_t base = usectime();
									int64_t offset = pts-Pa_GetStreamLast(stream, buffer_len, sample_rate);
									context.clock().sync(base, offset);
									iob.post("CLOCK base:%" PRId64 " offset:%" PRId64, base, offset);
								}
								shms[buffer_key]->free(index, [&](void *buffer){
										if(!context.is_resetting()) {
											context.sync_clock_as_needed(pts);
											Pa_WriteStream(stream, buffer, samples);
										}
									});

								reset_event(iob, [&]() {
										context.reset();
										return 0;
									});
								return 0;
							});
					});
			});
	}
}

int main(int argc, char **argv) {
	int device = 0;
	command_argument().require_full_argument("device", 'd', [&](const char *arg){
			sscanf(arg, "%d", &device);
			}).parse(argc, argv);
	iobus iob(stdin, stdout, stderr);
	return player_context::start(iob, [&](player_context &context) {
			return play_with_portaudio(iob, device, context);
		});
}

