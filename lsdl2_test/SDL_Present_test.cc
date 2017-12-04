#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_Present_test");

static SDL_Texture *arg_texture;
static SDL_Renderer *arg_renderer;
static uint8_t *arg_datas[3];
static int arg_lines[3];
static int arg_line_0, arg_line_1, arg_line_2;
static uint8_t *arg_data_0, *arg_data_1, *arg_data_2;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_lines[0] = arg_line_0 = 100;
	arg_lines[1] = arg_line_1 = 50;
	arg_lines[2] = arg_line_2 = 25;

	arg_datas[0] = arg_data_0 = (uint8_t *)10000;
	arg_datas[1] = arg_data_1 = (uint8_t *)100000;
	arg_datas[2] = arg_data_2 = (uint8_t *)1000000;

	arg_texture = (SDL_Texture *)&arg_texture;
	arg_renderer = (SDL_Renderer *)&arg_renderer;

	init_mock_function(SDL_UpdateYUVTexture);
	init_mock_function(SDL_RenderCopy);
	init_mock_function(SDL_RenderPresent);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_PresentYUV(arg_renderer, arg_texture, arg_datas, arg_lines);
}

SUITE_CASE("present image") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_UpdateYUVTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_UpdateYUVTexture, 1, arg_texture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_UpdateYUVTexture, 2, NULL);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_UpdateYUVTexture, 3, arg_data_0);
	CUE_EXPECT_CALLED_WITH_INT(SDL_UpdateYUVTexture, 4, arg_line_0);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_UpdateYUVTexture, 5, arg_data_1);
	CUE_EXPECT_CALLED_WITH_INT(SDL_UpdateYUVTexture, 6, arg_line_1);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_UpdateYUVTexture, 7, arg_data_2);
	CUE_EXPECT_CALLED_WITH_INT(SDL_UpdateYUVTexture, 8, arg_line_2);

	CUE_EXPECT_CALLED_ONCE(SDL_RenderCopy);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_RenderCopy, 1, arg_renderer);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_RenderCopy, 2, arg_texture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_RenderCopy, 3, NULL);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_RenderCopy, 4, NULL);

	CUE_EXPECT_CALLED_ONCE(SDL_RenderPresent);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_RenderPresent, 1, arg_renderer);
}

SUITE_CASE("failed to update texture") {
	init_mock_function_with_return(SDL_UpdateYUVTexture, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(SDL_RenderCopy);

	CUE_EXPECT_NEVER_CALLED(SDL_RenderPresent);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");
}

SUITE_CASE("failed to renderer copy") {
	init_mock_function_with_return(SDL_RenderCopy, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(SDL_RenderPresent);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");
}

SUITE_END(SDL_Present_test);

