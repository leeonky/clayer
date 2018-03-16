#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("TTF_OpenFont_test");

static TTF_Font *ret_font;
static const char *arg_font;
static int arg_fontsize;

mock_function_1(int, TTF_OpenFont_action, TTF_Font *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_font = (TTF_Font *)&ret_font;
	arg_font = "Font file";
	arg_fontsize = 20;

	init_mock_function(TTF_Init);
	init_mock_function_with_return(TTF_OpenFont, ret_font);
	init_mock_function(TTF_OpenFont_action);
	init_mock_function(TTF_CloseFont);
	init_mock_function(TTF_Quit);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return TTF_OpenFont(arg_font, arg_fontsize, TTF_OpenFont_action);
}

SUITE_CASE("open ttf font") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(TTF_Init);

	CUE_EXPECT_CALLED_ONCE(TTF_OpenFont);
	CUE_EXPECT_CALLED_WITH_STRING(TTF_OpenFont, 1, arg_font);
	CUE_EXPECT_CALLED_WITH_INT(TTF_OpenFont, 2, arg_fontsize);

	CUE_EXPECT_CALLED_ONCE(TTF_OpenFont_action);
	CUE_EXPECT_CALLED_WITH_PTR(TTF_OpenFont_action, 1, ret_font);

	CUE_EXPECT_CALLED_ONCE(TTF_CloseFont);
	CUE_EXPECT_CALLED_WITH_PTR(TTF_CloseFont, 1, ret_font);

	CUE_EXPECT_CALLED_ONCE(TTF_Quit);
}

SUITE_CASE("TTF init failed") {
	init_mock_function_with_return(TTF_Init, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(TTF_OpenFont);
	CUE_EXPECT_NEVER_CALLED(TTF_OpenFont_action);
	CUE_EXPECT_NEVER_CALLED(TTF_CloseFont);
}

SUITE_CASE("open ttf font failed") {
	init_mock_function_with_return(TTF_OpenFont, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(TTF_OpenFont_action);
	CUE_EXPECT_NEVER_CALLED(TTF_CloseFont);
}

SUITE_END(TTF_OpenFont_test);

