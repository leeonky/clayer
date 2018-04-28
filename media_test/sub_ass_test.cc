#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include <iostream>
#include <sstream> 
#include <string>
#include "stdexd/stdexd.h"
#include "media/sub_ass.h"
#include "mock_ass/mock_ass.h"

SUITE_START("sub_ass_test");

mock_void_function_2(ass_sub_action, ASS_Renderer *, ASS_Track *);

ASS_Library* ret_ass_lib;
ASS_Renderer* ret_ass_renderer;
ASS_Track ret_ass_track;
ASS_Image *ret_ass_image;
static const char *arg_ass_file, *arg_font_file;
static int arg_w, arg_h;
static int64_t arg_pts;

mock_void_function_1(ass_sub_render_action, ASS_Image *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_w = 100;
	arg_h = 40;
	arg_font_file = "font";
	arg_ass_file = "ass";
	ret_ass_lib = (ASS_Library *)&ret_ass_lib;
	ret_ass_renderer = (ASS_Renderer *)&ret_ass_renderer;
	ret_ass_image = (ASS_Image *)&ret_ass_image;

	arg_pts = 2000000;

	init_mock_function_with_return(ass_library_init, ret_ass_lib);
	init_mock_function_with_return(ass_renderer_init, ret_ass_renderer);
	init_mock_function(ass_set_frame_size);
	init_mock_function(ass_set_fonts);
	init_mock_function_with_return(ass_read_file, &ret_ass_track);
	init_mock_function(ass_sub_action);
	init_mock_function(ass_free_track);
	init_mock_function(ass_renderer_done);
	init_mock_function(ass_library_done);

	init_mock_function(ass_render_frame);
	init_mock_function(ass_sub_render_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return subtitle_ass(arg_ass_file, arg_w, arg_h, arg_font_file, ass_sub_action);
}

SUITE_CASE("init all") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(ass_library_init);

	CUE_EXPECT_CALLED_ONCE(ass_renderer_init);
	CUE_EXPECT_CALLED_WITH_PTR(ass_renderer_init, 1, ret_ass_lib);

	CUE_EXPECT_CALLED_ONCE(ass_set_frame_size);
	CUE_EXPECT_CALLED_WITH_PTR(ass_set_frame_size, 1, ret_ass_renderer);
	CUE_EXPECT_CALLED_WITH_INT(ass_set_frame_size, 2, arg_w);
	CUE_EXPECT_CALLED_WITH_INT(ass_set_frame_size, 3, arg_h);

	CUE_EXPECT_CALLED_ONCE(ass_set_fonts);
	CUE_EXPECT_CALLED_WITH_PTR(ass_set_fonts, 1, ret_ass_renderer);
	CUE_EXPECT_CALLED_WITH_STRING(ass_set_fonts, 2, arg_font_file);
	CUE_EXPECT_CALLED_WITH_PTR(ass_set_fonts, 3, NULL);
	CUE_EXPECT_CALLED_WITH_INT(ass_set_fonts, 4, 0);
	CUE_EXPECT_CALLED_WITH_PTR(ass_set_fonts, 5, NULL);
	CUE_EXPECT_CALLED_WITH_INT(ass_set_fonts, 6, 0);

	CUE_EXPECT_CALLED_ONCE(ass_read_file);
	CUE_EXPECT_CALLED_WITH_PTR(ass_read_file, 1, ret_ass_lib);
	CUE_EXPECT_CALLED_WITH_STRING(ass_read_file, 2, arg_ass_file);
	CUE_EXPECT_CALLED_WITH_STRING(ass_read_file, 3, "utf-8");

	CUE_EXPECT_CALLED_ONCE(ass_sub_action);
	CUE_EXPECT_CALLED_WITH_PTR(ass_sub_action, 1, ret_ass_renderer);
	CUE_EXPECT_CALLED_WITH_PTR(ass_sub_action, 2, &ret_ass_track);

	CUE_EXPECT_CALLED_ONCE(ass_free_track);
	CUE_EXPECT_CALLED_WITH_PTR(ass_free_track, 1, &ret_ass_track);

	CUE_EXPECT_CALLED_ONCE(ass_renderer_done);
	CUE_EXPECT_CALLED_WITH_PTR(ass_renderer_done, 1, ret_ass_renderer);

	CUE_EXPECT_CALLED_ONCE(ass_library_done);
	CUE_EXPECT_CALLED_WITH_PTR(ass_library_done, 1, ret_ass_lib);
}

SUITE_CASE("init ass lib failed") {
	init_mock_function_with_return(ass_library_init, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(ass_renderer_init);
	CUE_EXPECT_NEVER_CALLED(ass_set_frame_size);
	CUE_EXPECT_NEVER_CALLED(ass_set_fonts);
	CUE_EXPECT_NEVER_CALLED(ass_read_file);
	CUE_EXPECT_NEVER_CALLED(ass_sub_action);
	CUE_EXPECT_NEVER_CALLED(ass_free_track);
	CUE_EXPECT_NEVER_CALLED(ass_renderer_done);
	CUE_EXPECT_NEVER_CALLED(ass_library_done);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: ass_library_init failed\n");
}

SUITE_CASE("init renderer failed") {
	init_mock_function_with_return(ass_renderer_init, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(ass_set_frame_size);
	CUE_EXPECT_NEVER_CALLED(ass_set_fonts);
	CUE_EXPECT_NEVER_CALLED(ass_read_file);
	CUE_EXPECT_NEVER_CALLED(ass_sub_action);
	CUE_EXPECT_NEVER_CALLED(ass_free_track);
	CUE_EXPECT_NEVER_CALLED(ass_renderer_done);
	CUE_EXPECT_CALLED_ONCE(ass_library_done);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: ass_renderer_init failed\n");
}

SUITE_CASE("get sub track failed") {
	init_mock_function_with_return(ass_read_file, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(ass_sub_action);
	CUE_EXPECT_NEVER_CALLED(ass_free_track);
	CUE_EXPECT_CALLED_ONCE(ass_renderer_done);
	CUE_EXPECT_CALLED_ONCE(ass_library_done);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: ass_read_file failed\n");
}

SUITE_CASE("render subtitle in ms") {
	ass_render_frame(ret_ass_renderer, &ret_ass_track, arg_pts, ass_sub_render_action);

	CUE_EXPECT_CALLED_ONCE(ass_render_frame);
	CUE_EXPECT_CALLED_WITH_PTR(ass_render_frame, 1, ret_ass_renderer);
	CUE_EXPECT_CALLED_WITH_PTR(ass_render_frame, 2, &ret_ass_track);
	CUE_EXPECT_CALLED_WITH_INT(ass_render_frame, 3, arg_pts/1000);
}

static ASS_Image *stub_got_new_subtitle(ASS_Renderer *, ASS_Track *, long long, int *flag) {
	*flag = 1;
	return ret_ass_image;
}

SUITE_CASE("render subtitle when got new subtitle") {
	init_mock_function_with_function(ass_render_frame, stub_got_new_subtitle);
	ass_render_frame(ret_ass_renderer, &ret_ass_track, arg_pts, ass_sub_render_action);

	CUE_EXPECT_CALLED_ONCE(ass_sub_render_action);
	CUE_EXPECT_CALLED_WITH_PTR(ass_sub_render_action, 1, ret_ass_image);
}

static ASS_Image *stub_got_same_subtitle(ASS_Renderer *, ASS_Track *, long long, int *flag) {
	*flag = 0;
	return ret_ass_image;
}

SUITE_CASE("do not render subtitle when got same subtitle") {
	init_mock_function_with_function(ass_render_frame, stub_got_same_subtitle);
	ass_render_frame(ret_ass_renderer, &ret_ass_track, arg_pts, ass_sub_render_action);

	CUE_EXPECT_NEVER_CALLED(ass_sub_render_action);
}

SUITE_END(sub_ass_test);

