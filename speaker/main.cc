#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "lport_audio/lport_audio.h"
#include "media/media.h"

static int play_with_sdl2(iobus &iob, int device) {
	return audio_event(iob, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format){
			return SDL_OpenAudio(device, sample_rate, channels, AVSampleFormat_to_SDL(format), [&](SDL_AudioDeviceID device_id, const SDL_AudioSpec &audio_spec){
					return buffer_event(iob, [&](int shmid, size_t size, int count, int semid, int /*audio_buffer_key*/) {
						return circular_shm::load(shmid, size, count, semid,
							[&](circular_shm &shm){
						SDL_PauseAudioDevice(device_id, 0);
						while (!samples_event(iob, [&](sample_list &samples) {
								if(samples.count)
									iob.post("CLOCK base:%" PRId64 " offset:%" PRId64, usectime(), samples.samples[0].timestamp-SDL_AudioLast(device_id, audio_spec));
								for(int i=0; i<samples.count; i++){
									shm.free(samples.samples[i].index, [&](void *buffer){
										SDL_QueueAudio(device_id, buffer, audio_spec.channels, audio_spec.format, samples.samples[i].nb_samples);
										});
									wait_at_most(SDL_AudioLast(device_id, audio_spec), 400000);
								}
								return 0;
								}))
						;
						wait_at_most(SDL_AudioLast(device_id, audio_spec), 0);
						return 0;
						});
					});
				});
			});
}

static int play_with_portaudio(iobus &iob, int device) {
	return audio_event(iob, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format){
			return buffer_event(iob, [&](int shmid, size_t size, int count, int semid, int /*audio_buffer_key*/) {
				return circular_shm::load(shmid, size, count, semid, [&](circular_shm &shm){
					return Pa_Init_OpenOutputStream(device, sample_rate, channels, AVSampleFormat_to_PortAudio(format), [&](PaStream *stream){
						long buffer_len = Pa_GetStreamWriteAvailable(stream);
						while (!samples_event(iob, [&](sample_list &samples) {
								if(samples.count)
									iob.post("CLOCK base:%" PRId64 " offset:%" PRId64, usectime(), samples.samples[0].timestamp-Pa_GetStreamLast(stream, buffer_len, sample_rate));
								for(int i=0; i<samples.count; i++)
									shm.free(samples.samples[i].index, [&](void *buffer){
										Pa_WriteStream(stream, buffer, samples.samples[i].nb_samples);
										});
								return 0;
								}))
						;
						return 0;
						});
					});
				});
			});
}

int main(int argc, char **argv) {
	int device = 0;
	command_argument().require_full_argument("device", 'd', [&](const char *arg){
			sscanf(arg, "%d", &device);
			}).parse(argc, argv);
	iobus iob(stdin, stdout, stderr);
	return play_with_portaudio(iob, device);
}

