#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(SDL_CreateWindow_test);
	ADD_SUITE(SDL_CreateTexture_test);
	ADD_SUITE(SDL_Present_test);
	return run_test();
}


