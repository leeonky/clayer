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

SUITE_START("samples_event_test");

mock_function_1(int, samples_event_action, sample_list*);

int samples_event_action_ref(sample_list &list) {
	return samples_event_action(&list);
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
	return samples_event(iob, samples_event_action_ref);
}

static int assert_sample_list_0(sample_list *list) {
	CUE_ASSERT_EQ(list->count, 0);
	return 0;
}

SUITE_CASE("enmpty SAMPLES") {
	init_subject("SAMPLES buffer:5");
	init_mock_function_with_function(samples_event_action, assert_sample_list_0);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(samples_event_action);
}

static int assert_sample_list_1(sample_list *list) {
	CUE_ASSERT_EQ(list->count, 1);
	CUE_ASSERT_EQ(list->buffer_key, 5);
	CUE_ASSERT_EQ(list->samples[0].index, 1);
	CUE_ASSERT_EQ(list->samples[0].timestamp, 0);
	CUE_ASSERT_EQ(list->samples[0].nb_samples, 480);
	return 0;
}

SUITE_CASE("one sample") {
	init_subject("SAMPLES buffer:5 1=>0,480");
	init_mock_function_with_function(samples_event_action, assert_sample_list_1);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(samples_event_action);
}

static int assert_sample_list_2(sample_list *list) {
	CUE_ASSERT_EQ(list->count, 2);
	CUE_ASSERT_EQ(list->samples[0].index, 1);
	CUE_ASSERT_EQ(list->samples[0].timestamp, 0);
	CUE_ASSERT_EQ(list->samples[0].nb_samples, 480);
	CUE_ASSERT_EQ(list->samples[1].index, 2);
	CUE_ASSERT_EQ(list->samples[1].timestamp, 300);
	CUE_ASSERT_EQ(list->samples[1].nb_samples, 400);
	return 0;
}

SUITE_CASE("two samples") {
	init_subject("SAMPLES buffer:5 1=>0,480 2=>300,400");
	init_mock_function_with_function(samples_event_action, assert_sample_list_2);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(samples_event_action);
}

SUITE_END(samples_event_test);

