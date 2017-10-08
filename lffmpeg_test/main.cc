#include <cstdlib>
#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(avformat_open_close_test);
	ADD_SUITE(avformat_find_stream_test);
	ADD_SUITE(avstream_info_test);
	ADD_SUITE(avcodec_open_test);
	ADD_SUITE(av_new_packet_test);
	return run_test();
}


