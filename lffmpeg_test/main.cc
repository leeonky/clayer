#include <cstdlib>
#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(avformat_open_close_test);
	return run_test();
}


