#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_CreateTexture_only_test");

static SDL_Renderer *arg_renderer;
static SDL_Texture *ret_texture;
static int arg_w, arg_h, arg_access;
static Uint32 arg_format;

namespace {
mock_function_1(int, sdl_texture_action, SDL_Texture *);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_renderer = (SDL_Renderer *)&arg_renderer;
	ret_texture = (SDL_Texture *)&ret_texture;
	arg_w = 800;
	arg_h = 600;
	arg_format = 1024;
	arg_access = 100;

	init_mock_function_with_return(SDL_CreateTexture, ret_texture);
	init_mock_function(sdl_texture_action);
	init_mock_function(SDL_DestroyTexture);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_CreateTexture(arg_renderer, arg_format, arg_access, arg_w, arg_h, sdl_texture_action);
}

SUITE_CASE("create sdl texture") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_CreateTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_CreateTexture, 1, arg_renderer);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 2, arg_format);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 3, arg_access);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 4, arg_w);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 5, arg_h);

	CUE_EXPECT_CALLED_ONCE(sdl_texture_action);
	CUE_EXPECT_CALLED_WITH_PTR(sdl_texture_action, 1, ret_texture);

	CUE_EXPECT_CALLED_ONCE(SDL_DestroyTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_DestroyTexture, 1, ret_texture);
}

SUITE_CASE("failed to create texture") {
	init_mock_function_with_return(SDL_CreateTexture, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(sdl_texture_action);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyTexture);
}

SUITE_END(SDL_CreateTexture_only_test);
