#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_CreateWindow_test");

static SDL_Window *arg_window;
static SDL_Renderer *ret_renderer;
static SDL_Texture *ret_texture;
static int ret_width, ret_height;
static int arg_width, arg_height;
static Uint32 arg_format;

namespace {
mock_function_4(int, sdl_texture_action, int, int, SDL_Renderer *, SDL_Texture *);
}

//static void stub_SDL_GL_GetDrawableSize(SDL_Window *window, int *w, int *h) {
	//*w = ret_width;
	//*h = ret_height;
//}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_window = (SDL_Window *)&arg_window;
	ret_renderer = (SDL_Renderer *)&ret_renderer;
	ret_texture = (SDL_Texture *)&ret_texture;
	arg_width = ret_width = 800;
	arg_height = ret_height = 600;
	arg_format = 1024;

	init_mock_function_with_return(SDL_CreateRenderer, ret_renderer);
	//init_mock_function_with_function(SDL_GL_GetDrawableSize, stub_SDL_GL_GetDrawableSize);
	init_mock_function_with_return(SDL_CreateTexture, ret_texture);
	init_mock_function(sdl_texture_action);
	init_mock_function(SDL_DestroyTexture);
	init_mock_function(SDL_DestroyRenderer);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_CreateTexture(arg_window, arg_width, arg_height, arg_format, sdl_texture_action);
}

SUITE_CASE("create sdl texture") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_CreateRenderer);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_CreateRenderer, 1, arg_window);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateRenderer, 2, -1);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateRenderer, 3, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//CUE_EXPECT_CALLED_ONCE(SDL_GL_GetDrawableSize);
	//CUE_EXPECT_CALLED_WITH_PTR(SDL_GL_GetDrawableSize, 1, arg_window);

	CUE_EXPECT_CALLED_ONCE(SDL_CreateTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_CreateTexture, 1, ret_renderer);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 2, arg_format);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 3, SDL_TEXTUREACCESS_STREAMING);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 4, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 5, arg_height);

	CUE_EXPECT_CALLED_ONCE(sdl_texture_action);
	CUE_EXPECT_CALLED_WITH_INT(sdl_texture_action, 1, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(sdl_texture_action, 2, arg_height);
	CUE_EXPECT_CALLED_WITH_PTR(sdl_texture_action, 3, ret_renderer);
	CUE_EXPECT_CALLED_WITH_PTR(sdl_texture_action, 4, ret_texture);

	CUE_EXPECT_CALLED_ONCE(SDL_DestroyTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_DestroyTexture, 1, ret_texture);

	CUE_EXPECT_CALLED_ONCE(SDL_DestroyRenderer);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_DestroyRenderer, 1, ret_renderer);
}

SUITE_CASE("failed to create renderer") {
	init_mock_function_with_return(SDL_CreateRenderer, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(SDL_CreateTexture);
	CUE_EXPECT_NEVER_CALLED(sdl_texture_action);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyTexture);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyRenderer);
}

SUITE_CASE("failed to create texture") {
	init_mock_function_with_return(SDL_CreateTexture, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(sdl_texture_action);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyTexture);
}

SUITE_CASE("use min width and height of input and window") {
	arg_width = 400; ret_width = 800;
	arg_height = 300; ret_height = 600;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_CreateTexture);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 4, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 5, arg_height);

	CUE_EXPECT_CALLED_ONCE(sdl_texture_action);
	CUE_EXPECT_CALLED_WITH_INT(sdl_texture_action, 1, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(sdl_texture_action, 2, arg_height);
}

SUITE_END(SDL_CreateTexture_test);
