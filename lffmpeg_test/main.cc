#include <cstdlib>
#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(avformat_open_close_test);
	ADD_SUITE(avformat_find_stream_test);
	ADD_SUITE(avstream_info_test);
	return run_test();
}


