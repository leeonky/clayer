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

	int play_with_portaudio(iobus &iob, int device) {
		return forward_untill(iob, audio_event, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format){
				return Pa_Init_OpenOutputStream(device, sample_rate, channels, AVSampleFormat_to_PortAudio(format), [&](PaStream *stream){
						long buffer_len = Pa_GetStreamWriteAvailable(stream);
						return msgget([&](int msgid) {
								iob.post("CONTROL id:%d", msgid);
								return main_reducer(iob, shms, sample_event, [&](int buffer_key, int index, int64_t pts, int samples) {
									player_command_process(msgid, 0, [&] (const char *) { return 0; });
									if(samples)
										iob.post("CLOCK base:%" PRId64 " offset:%" PRId64, usectime(), pts-Pa_GetStreamLast(stream, buffer_len, sample_rate));
									shms[buffer_key]->free(index, [&](void *buffer){
										Pa_WriteStream(stream, buffer, samples);
										});
									return 0;
									});
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
	return play_with_portaudio(iob, device);
}

