#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(SDL_CreateWindow_test);
	ADD_SUITE(SDL_CreateTexture_test);
	ADD_SUITE(SDL_Present_test);
	ADD_SUITE(SDL_OpenAudio_test);
	ADD_SUITE(SDL_QueueAudio_test);
	ADD_SUITE(SDL_AudioLast_test);
	ADD_SUITE(SDL_AudioClock_test);
	return run_test();
}


