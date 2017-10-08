#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_new_packet_test");

static AVPacket *ret_av_packet;

mock_function_1(int, av_new_packet_action, AVPacket *);

static int av_new_packet_action_ref(AVPacket &av_packet) {
	return av_new_packet_action(&av_packet);
}

BEFORE_EACH() {
	init_mock_function(av_init_packet);
	init_mock_function(av_packet_unref);
	return 0;
}

SUBJECT(int) {
	return av_new_packet(av_new_packet_action_ref);
}

static int av_new_packet_action_assign_value(AVPacket *av_packet) {
	ret_av_packet = av_packet;
	return 0;
}

SUITE_CASE("init AVPacket") {
	init_mock_function_with_function(av_new_packet_action, av_new_packet_action_assign_value);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_init_packet);
	CUE_EXPECT_CALLED_WITH_PTR(av_init_packet, 1, ret_av_packet);

	CUE_EXPECT_CALLED_ONCE(av_packet_unref);
	CUE_EXPECT_CALLED_WITH_PTR(av_packet_unref, 1, ret_av_packet);
}

SUITE_END(av_new_packet_test);
