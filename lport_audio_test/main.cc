#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(Pa_Init_and_OpenStream_test);
	ADD_SUITE(Pa_GetStreamLast_test);
	return run_test();
}



