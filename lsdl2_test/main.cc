#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(SDL_CreateWindow_test);
	return run_test();
}


