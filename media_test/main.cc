#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(ffmpeg_to_sdl_format);
	ADD_SUITE(video_event_test);
	ADD_SUITE(buffer_event_test);
	ADD_SUITE(frames_event_test);
	return run_test();
}

