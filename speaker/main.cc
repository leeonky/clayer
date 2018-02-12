#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

int main(int, char **) {
	iobus iob(stdin, stdout, stderr);

	return audio_event(iob, [&](int sample_rate, int channels, int64_t /*layout*/, enum AVSampleFormat format){
			return SDL_OpenAudio(0, sample_rate, channels, AVSampleFormat_to_SDL(format), [&](SDL_AudioDeviceID device_id, const SDL_AudioSpec &audio_spec){
					return buffer_event(iob, [&](int shmid, size_t size, int count, int semid) {
						return circular_shm::load(shmid, size, count, semid,
							[&](circular_shm &shm){
						SDL_PauseAudioDevice(device_id, 0);
						while (!samples_event(iob, [&](sample_list &samples) {
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

