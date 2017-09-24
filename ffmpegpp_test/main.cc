#include <cstdlib>
#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(ffmpeg_open_test);
	return run_test();
}

