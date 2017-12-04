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

SUITE_START("open_video_test");

static SDL_Window *ret_window;
static const char *arg_title;
static int arg_width, arg_height, arg_x, arg_y;
static Uint32 arg_flag;

static SDL_Renderer *ret_renderer;
static SDL_Texture *ret_texture;

mock_function_5(int, video_action, SDL_Window *, SDL_Renderer *, int, int, SDL_Texture *);

std::unique_ptr<iobus> iob;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	iob.reset(new iobus(actxt.input_stream, actxt.output_stream, actxt.error_stream));

	ret_window = (SDL_Window *)&ret_window;
	ret_renderer = (SDL_Renderer *)&ret_renderer;
	ret_texture = (SDL_Texture *)&ret_texture;

	arg_title = "title";
	arg_x = 100;
	arg_y = 200;
	arg_width = 800;
	arg_height = 600;
	arg_flag = 8;

	init_mock_function_with_function(SDL_InitSubSystem, NULL);
	init_mock_function_with_return(SDL_CreateWindow, ret_window);
	init_mock_function_with_function(SDL_DestroyWindow, NULL);
	init_mock_function_with_function(SDL_QuitSubSystem, NULL);
	init_mock_function_with_function(SDL_ShowCursor, NULL);

	init_mock_function_with_return(SDL_CreateRenderer, ret_renderer);
	init_mock_function_with_return(SDL_CreateTexture, ret_texture);
	init_mock_function(SDL_DestroyTexture);
	init_mock_function(SDL_DestroyRenderer);
	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_YUV420P);

	init_mock_function(video_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return open_video(*iob, arg_title, arg_x, arg_y, arg_width, arg_height, arg_flag, video_action);
}

SUITE_CASE("create sdl texture") {
	init_subject("VIDEO width:1920 height:1080 format:yuv420p");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_get_pix_fmt);

	CUE_EXPECT_CALLED_ONCE(SDL_CreateWindow);
	CUE_EXPECT_CALLED_WITH_STRING(SDL_CreateWindow, 1, arg_title);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 2, arg_x);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 3, arg_y);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 4, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 5, arg_height);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 6, arg_flag);

	CUE_EXPECT_CALLED_ONCE(SDL_CreateTexture);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_CreateTexture, 1, ret_renderer);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 2, SDL_PIXELFORMAT_IYUV);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 4, 1920);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateTexture, 5, 1080);

	CUE_EXPECT_CALLED_ONCE(video_action);
	CUE_EXPECT_CALLED_WITH_PTR(video_action, 1, ret_window);
	CUE_EXPECT_CALLED_WITH_PTR(video_action, 2, ret_renderer);
	CUE_EXPECT_CALLED_WITH_INT(video_action, 3, 1920);
	CUE_EXPECT_CALLED_WITH_INT(video_action, 4, 1080);
	CUE_EXPECT_CALLED_WITH_PTR(video_action, 5, ret_texture);
}

//event not match
//args not match
//open failed
//ffmpeg format => sdl format

SUITE_END(open_video_test);

