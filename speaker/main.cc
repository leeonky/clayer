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
	int sync_interval = 16;

	int play_with_portaudio(iobus &iob, int device, player_context &context) {
		return forward_untill(iob, audio_event, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format, int /*passthrough*/){
				return Pa_Init_OpenOutputStream(device, sample_rate, channels, AVSampleFormat_to_PortAudio(format), [&](PaStream *stream){
						long buffer_len = Pa_GetStreamWriteAvailable(stream);
						return main_reducer(iob, shms, sample_event, [&](int buffer_key, int index, int64_t pts, int samples) {
								context.process_command();
								if(context.scaling_down(sync_interval)) {
									int64_t base = usectime();
									int64_t offset = pts-Pa_GetStreamLast(stream, buffer_len, sample_rate);
									context.clock().sync(base, offset);
									context.post_clock_event(iob);
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

	const char *guess_device(const char *device, const char *device_passthrough, int passthrough) {
		if(passthrough) {
			if(strlen(device_passthrough))
				return device_passthrough;
			else {
				static char buffer[256] = "";
				sprintf(buffer, "%s,AES0=0x6", device);
				return buffer;
			}
		} else
			return device;
	}

	int play_with_alsa(iobus &iob, const char *device, const char *device_passthrough, player_context &context) {
		return forward_untill(iob, audio_event, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format, int passthrough){
				return snd_pcm_open(guess_device(device, device_passthrough, passthrough),
						sample_rate, channels, AVSampleFormat_to_ALSA(format), [&] (snd_pcm_t *pcm) {
						return main_reducer(iob, shms, sample_event, [&](int buffer_key, int index, int64_t pts, int samples) {
								context.process_command();
								if(context.scaling_down(sync_interval)) {
									int64_t base = usectime();
									int64_t offset = pts-snd_pcm_delay(pcm, sample_rate);
									context.clock().sync(base, offset);
									context.post_clock_event(iob);
								}
								shms[buffer_key]->free(index, [&](void *buffer){
										if(!context.is_resetting()) {
											context.sync_clock_as_needed(pts);
											snd_pcm_mmap_writei(pcm, buffer, samples);
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
	char alsa_device[128] = "";
	char alsa_passthrough[128] = "";
	command_argument().require_full_argument("clock", 'c', [&](const char *arg){
			sscanf(arg, "%d", &sync_interval);
			}).require_full_argument("port", 't', [&](const char *arg){
			sscanf(arg, "%d", &device);
			}).require_full_argument("alsa", 'a', [&](const char *arg){
			sscanf(arg, "%s", alsa_device);
			}).require_full_argument("alsa_pass", 'p', [&](const char *arg){
			sscanf(arg, "%s", alsa_passthrough);
			}).parse(argc, argv);
	iobus iob(stdin, stdout, stderr);
	return player_context::start(iob, [&](player_context &context) {
			if(strlen(alsa_device) || strlen(alsa_passthrough))
				return play_with_alsa(iob, alsa_device, alsa_passthrough, context);
			else
				return play_with_portaudio(iob, device, context);
		});
}

