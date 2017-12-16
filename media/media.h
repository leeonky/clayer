#ifndef MEDIA_H
#define MEDIA_H

#include <functional>
#include "iobus/iobus.h"
#include "lffmpeg/lffmpeg.h"
#include "lsdl2/lsdl2.h"
#include "stdport/stdport.h"

Uint32 AVPixelFormat_to_SDL(enum AVPixelFormat);
int video_event(iobus &, const std::function<int(int, int, enum AVPixelFormat)> &);
int buffer_event(iobus &, const std::function<int(int, size_t, int, int)> &);
int audio_event(iobus &, const std::function<int(int, int, int64_t, enum AVSampleFormat)> &);

#define MAX_FRAMES_COUNT 128
struct frame_list {
	struct frame {
		int index;
		int64_t timestamp;
	} frames[MAX_FRAMES_COUNT];
	int count = 0;
};
int frames_event(iobus &, const std::function<int(frame_list &)> &);

class media_clock {
public:
	media_clock(): base(usectime()), offset() {}
	int wait(int64_t, int64_t);
	void sync(int64_t base, int64_t offset) {
		this->base = base;
		this->offset = offset;
	}
private:
	int64_t base, offset;
};
#endif
