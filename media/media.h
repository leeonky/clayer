#ifndef MEDIA_H
#define MEDIA_H

#include <functional>
#include <vector>
#include <portaudio.h>
#include "iobus/iobus.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "lsdl2/lsdl2.h"
#include "lalsa/lalsa.h"
#include "stdport/stdport.h"

Uint32 AVPixelFormat_to_SDL(enum AVPixelFormat);
SDL_AudioFormat AVSampleFormat_to_SDL(enum AVSampleFormat);
PaSampleFormat AVSampleFormat_to_PortAudio(enum AVSampleFormat);
snd_pcm_format_t AVSampleFormat_to_ALSA(enum AVSampleFormat);

int video_event(iobus &, const std::function<int(int, int, enum AVPixelFormat)> &);
int buffer_event(iobus &, const std::function<int(int, size_t, int, int, int)> &);
int audio_event(iobus &, const std::function<int(int, int, int64_t, enum AVSampleFormat, int)> &);
int clock_event(iobus &, const std::function<int(int64_t, int64_t)> &);

#define MAX_FRAMES_COUNT 128
struct frame_list {
	int buffer_key;
	struct frame {
		int index;
		int64_t timestamp;
	} frames[MAX_FRAMES_COUNT];
	int count = 0;
};
int frames_event(iobus &, const std::function<int(frame_list &)> &);
int frame_event(iobus &, const std::function<int(int, int, int64_t)> &);

#define MAX_SAMPLES_COUNT 128
struct sample_list {
	int buffer_key;
	struct sample {
		int index;
		int64_t timestamp;
		int nb_samples;
	} samples[MAX_SAMPLES_COUNT];
	int count = 0;
};
int samples_event(iobus &, const std::function<int(sample_list &)> &);
int sample_event(iobus &, const std::function<int(int, int, int64_t, int)> &);

class media_clock {
public:
	media_clock(): base(usectime()), offset() {}
	int wait(int64_t, int64_t);

	void sync(int64_t base, int64_t offset) {
		this->base = base;
		this->offset = offset;
	}

	int64_t now() const {
		return usectime()-base+offset;
	}

	void backward(int64_t offset) {
		base += offset;
	}
	int64_t base, offset;
};

extern void wait_at_most(int64_t, int64_t);

extern int64_t analyze_channel_layout(int64_t, const char *);

extern int analyze_sample_rate(int, const char *);

extern enum AVSampleFormat analyze_sample_format(enum AVSampleFormat, const char *);

#define MAX_LAYER_COUNT	32

#define MAX_SUB_LAYER_COUNT 128
struct layer_list {
	int buffer_key;
	int index;
	int id;
	int width, height;
	struct sub_layer {
		int x, y, w, h, offset, pitch;
	} sub_layers[MAX_SUB_LAYER_COUNT];
	int count;
};

extern int layer_event(iobus &, const std::function<int(const layer_list &)> &);
extern int nolayer_event(iobus &, const std::function<int(int)> &);
extern int control_event(iobus &, const std::function<int(int)> &);
extern int reset_event(iobus &, const std::function<int(void)> &);

template<typename Processor, typename Action>
int main_reducer(iobus &iob, circular_shm **shms, const Processor &main_processor, const Action &action) {
	std::function<int(int, size_t, int, int, int)> buffer_action = [&](int shmid, size_t size, int count, int semid, int buffer_key) {
		return circular_shm::load(shmid, size, count, semid,
			[&](circular_shm &shm){
			shms[buffer_key] = &shm;

			while(buffer_event(iob, buffer_action)) {
				if(!main_processor(iob, action)) {
					while(!ignore_untill(iob, main_processor, action))
						;
					break;
				} else
					iob.ignore_last();
			}
			return 0;
			});
		};
	return ignore_untill(iob, buffer_event, buffer_action);
}

template<typename Processor, typename Action>
int main_transform(iobus &iob, circular_shm **shms, const Processor &main_processor, const Action &action) {
	std::function<int(int, size_t, int, int, int)> buffer_action = [&](int shmid, size_t size, int count, int semid, int buffer_key) {
		return circular_shm::load(shmid, size, count, semid,
			[&](circular_shm &shm){
			shms[buffer_key] = &shm;
			iob.recaption_and_post();

			while(buffer_event(iob, buffer_action)) {
				if(!main_processor(iob, action)) {
					while(!forward_untill(iob, main_processor, action))
						;
					break;
				} else
					iob.forward_last();
			}
			return 0;
			});
		};
	return forward_untill(iob, buffer_event, buffer_action);
}

class player_context {
public:
	static int start(iobus &, const std::function<int(player_context &)> &);
	void process_command();

	media_clock &clock() {
		return _clock;
	}

	void reset() {
		_resetting = false;
	}

	bool is_resetting() const {
		return _resetting;
	}

	void sync_clock_as_needed(int64_t pts) {
		if(_need_sync) {
			_clock.sync(usectime(), pts);
			_need_sync = false;
		}
	}

	bool scaling_down(int interval) {
		return !(_sample_seq++ % interval);
	}

	void post_clock_event(iobus &iob) {
		iob.post("CLOCK base:%" PRId64 " offset:%" PRId64, _clock.base, _clock.offset);
	}

private:
	media_clock _clock;
	bool _resetting, _need_sync;
	int _msgid, _receiver;
	int _sample_seq;

	player_context(int msgid, int receiver) :_resetting(false), _need_sync(false), _msgid(msgid), _receiver(receiver), _sample_seq(0) {}
};

extern enum AVPixelFormat analyze_pixel_format(enum AVPixelFormat, const char *, const char *);

extern void analyze_size_format(int &, int &, const char *);

#endif

