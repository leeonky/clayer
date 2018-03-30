#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

static int64_t arg_in_w, arg_in_h, arg_out_w, arg_out_h;
static enum AVPixelFormat arg_in_format, arg_out_format;
static int arg_flag;

static SwsContext *ret_sws_context;

mock_function_1(int, sws_new_action, scale_context *);

int sws_new_action_ref(scale_context &context) {
	return sws_new_action(&context);
}

SUITE_START("sws_getContext_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_sws_context = (SwsContext *)&ret_sws_context;

	arg_in_w = 100;
	arg_in_h = 200;
	arg_in_format = AV_PIX_FMT_YUV420P;

	arg_out_w = 1000;
	arg_out_h = 2000;
	arg_out_format = AV_PIX_FMT_YUV444P;

	init_mock_function(sws_new_action);
	init_mock_function_with_return(sws_getContext, ret_sws_context);
	init_mock_function(sws_freeContext);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return sws_getContext(arg_in_w, arg_in_h, arg_in_format, arg_out_w, arg_out_h, arg_out_format, arg_flag, sws_new_action_ref);
}

static int assert_sws_context(scale_context *context) {
	CUE_ASSERT_EQ(context->in_w, arg_in_w);
	CUE_ASSERT_EQ(context->in_h, arg_in_h);
	CUE_ASSERT_EQ(context->in_format, arg_in_format);
	CUE_ASSERT_EQ(context->out_w, arg_out_w);
	CUE_ASSERT_EQ(context->out_h, arg_out_h);
	CUE_ASSERT_EQ(context->out_format, arg_out_format);
	CUE_ASSERT_EQ(context->flag, arg_flag);
	CUE_ASSERT_PTR_EQ(context->sws_context, ret_sws_context);
	return 0;
}

SUITE_CASE("create scale_context") {
	init_mock_function_with_function(sws_new_action, assert_sws_context);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(sws_getContext);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 1, arg_in_w);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 2, arg_in_h);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 3, arg_in_format);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 4, arg_out_w);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 5, arg_out_h);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 6, arg_out_format);
	CUE_EXPECT_CALLED_WITH_INT(sws_getContext, 7, arg_flag);
	CUE_EXPECT_CALLED_WITH_PTR(sws_getContext, 8, NULL);
	CUE_EXPECT_CALLED_WITH_PTR(sws_getContext, 9, NULL);
	CUE_EXPECT_CALLED_WITH_PTR(sws_getContext, 10, NULL);

	CUE_EXPECT_CALLED_ONCE(sws_new_action);

	CUE_EXPECT_CALLED_ONCE(sws_freeContext);
	CUE_EXPECT_CALLED_WITH_PTR(sws_freeContext, 1, ret_sws_context);
}

SUITE_CASE("failed create object") {
	init_mock_function_with_function(sws_getContext, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(sws_new_action);

	CUE_EXPECT_NEVER_CALLED(sws_freeContext);
}

SUITE_END(sws_getContext_test);

