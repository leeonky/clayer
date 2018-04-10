#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(snd_pcm_open_test);
	ADD_SUITE(snd_pcm_mmap_writei_test);
	ADD_SUITE(snd_pcm_delay_test);
	return run_test();
}

