#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(ffmpeg_to_sdl_format);
	ADD_SUITE(open_video_test);
	ADD_SUITE(load_buffer_test);
	return run_test();
}

