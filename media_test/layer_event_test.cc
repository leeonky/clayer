#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "lffmpeg/lffmpeg.h"
#include "media/media.h"
#include "mock_sdl2/mock_sdl2.h"
#include "mock_ffmpeg/mock_ffmpeg.h"

SUITE_START("layer_event_test");

mock_function_1(int, layer_event_action, const layer_list *);

int layer_event_action_ref(const layer_list &list) {
	return layer_event_action(&list);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return layer_event(iob, layer_event_action_ref);
}

static int layer_event_action_assert(const layer_list *list) {
	CUE_ASSERT_EQ(list->buffer_key, 5);
	CUE_ASSERT_EQ(list->index, 9);
	CUE_ASSERT_EQ(list->count, 1);
	CUE_ASSERT_EQ(list->id, 8);
	CUE_ASSERT_EQ(list->width, 1920);
	CUE_ASSERT_EQ(list->height, 817);
	CUE_ASSERT_EQ(list->sub_layers[0].offset, 0);
	CUE_ASSERT_EQ(list->sub_layers[0].x, 10);
	CUE_ASSERT_EQ(list->sub_layers[0].y, 20);
	CUE_ASSERT_EQ(list->sub_layers[0].w, 400);
	CUE_ASSERT_EQ(list->sub_layers[0].h, 300);
	CUE_ASSERT_EQ(list->sub_layers[0].pitch, 1000);
	return 0;
}

SUITE_CASE("get layer list") {
	init_mock_function_with_function(layer_event_action, layer_event_action_assert);
	init_subject("LAYER buffer:5 index:9 id:8 width:1920 height:817 0=>10,20,400,300,1000");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(layer_event_action);
}

static int layer_event_action_assert2(const layer_list *list) {
	CUE_ASSERT_EQ(list->buffer_key, 5);
	CUE_ASSERT_EQ(list->index, 9);
	CUE_ASSERT_EQ(list->count, 2);
	CUE_ASSERT_EQ(list->id, 3);
	CUE_ASSERT_EQ(list->width, 1920);
	CUE_ASSERT_EQ(list->height, 817);
	CUE_ASSERT_EQ(list->sub_layers[0].offset, 0);
	CUE_ASSERT_EQ(list->sub_layers[0].x, 10);
	CUE_ASSERT_EQ(list->sub_layers[0].y, 20);
	CUE_ASSERT_EQ(list->sub_layers[0].w, 400);
	CUE_ASSERT_EQ(list->sub_layers[0].h, 300);
	CUE_ASSERT_EQ(list->sub_layers[0].pitch, 1000);
	CUE_ASSERT_EQ(list->sub_layers[1].offset, 1);
	CUE_ASSERT_EQ(list->sub_layers[1].x, 11);
	CUE_ASSERT_EQ(list->sub_layers[1].y, 21);
	CUE_ASSERT_EQ(list->sub_layers[1].w, 401);
	CUE_ASSERT_EQ(list->sub_layers[1].h, 301);
	CUE_ASSERT_EQ(list->sub_layers[1].pitch, 2000);
	return 0;
}

SUITE_CASE("get layer list with two sub_layers") {
	init_mock_function_with_function(layer_event_action, layer_event_action_assert2);
	init_subject("LAYER buffer:5 index:9 id:3 width:1920 height:817 0=>10,20,400,300,1000 1=>11,21,401,301,2000");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(layer_event_action);
}

SUITE_CASE("event not match") {
	init_subject("AUDIO 1=>0");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
}

SUITE_END(layer_event_test);

