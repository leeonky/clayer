#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("TTF_RenderUTF8_Blended_test");

static TTF_Font *arg_font;
static const char *arg_text;
static SDL_Color arg_color;
static SDL_Surface *ret_surface;

mock_function_1(int, TTF_RenderUTF8_Blended_action, SDL_Surface *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_font = (TTF_Font *)&arg_font;
	arg_text = "Hello";
	arg_color.r = 0xff;
	ret_surface = (SDL_Surface *)&ret_surface;

	init_mock_function_with_return(TTF_RenderUTF8_Blended, ret_surface);
	init_mock_function(TTF_RenderUTF8_Blended_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return TTF_RenderUTF8_Blended(arg_font, arg_text, arg_color, TTF_RenderUTF8_Blended_action);
}

SUITE_CASE("open ttf font") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(TTF_RenderUTF8_Blended);
	CUE_EXPECT_CALLED_WITH_PTR(TTF_RenderUTF8_Blended, 1, arg_font);
	CUE_EXPECT_CALLED_WITH_STRING(TTF_RenderUTF8_Blended, 2, arg_text);

	CUE_EXPECT_CALLED_ONCE(TTF_RenderUTF8_Blended_action);
	CUE_EXPECT_CALLED_WITH_PTR(TTF_RenderUTF8_Blended_action, 1, ret_surface);
}

SUITE_CASE("Render failed") {
	init_mock_function_with_return(TTF_RenderUTF8_Blended, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(TTF_RenderUTF8_Blended_action);
}

SUITE_END(TTF_RenderUTF8_Blended_test);


