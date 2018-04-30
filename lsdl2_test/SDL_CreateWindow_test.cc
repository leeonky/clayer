#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_CreateWindow_test");

static SDL_Window *ret_window;
static const char *arg_title;
static int arg_width, arg_height, arg_x, arg_y;
static Uint32 arg_flag;
mock_function_1(int, sdl_window_action, SDL_Window *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_window = (SDL_Window *)&ret_window;
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
	init_mock_function(sdl_window_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_CreateWindow(arg_title, arg_x, arg_y, arg_width, arg_height, arg_flag, sdl_window_action);
}

SUITE_CASE("create sdl window") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_InitSubSystem);
	CUE_EXPECT_CALLED_WITH_INT(SDL_InitSubSystem, 1, SDL_INIT_VIDEO);

	CUE_EXPECT_CALLED_ONCE(SDL_CreateWindow);
	CUE_EXPECT_CALLED_WITH_STRING(SDL_CreateWindow, 1, arg_title);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 2, arg_x);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 3, arg_y);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 4, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 5, arg_height);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CreateWindow, 6, arg_flag);

	CUE_EXPECT_CALLED_ONCE(SDL_DestroyWindow);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_DestroyWindow, 1, ret_window);

	CUE_EXPECT_CALLED_ONCE(SDL_QuitSubSystem);
	CUE_EXPECT_CALLED_WITH_INT(SDL_QuitSubSystem, 1, SDL_INIT_VIDEO);

	CUE_EXPECT_CALLED_ONCE(sdl_window_action);
	CUE_EXPECT_CALLED_WITH_INT(SDL_QuitSubSystem, 1, SDL_INIT_VIDEO);
}

SUITE_CASE("failed to init video") {
	init_mock_function_with_return(SDL_InitSubSystem, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(SDL_CreateWindow);
	CUE_EXPECT_NEVER_CALLED(sdl_window_action);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyWindow);
	CUE_EXPECT_NEVER_CALLED(SDL_QuitSubSystem);
}

SUITE_CASE("failed to create window") {
	init_mock_function_with_return(SDL_CreateWindow, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");

	CUE_EXPECT_NEVER_CALLED(sdl_window_action);
	CUE_EXPECT_NEVER_CALLED(SDL_DestroyWindow);
}

SUITE_END(SDL_CreateWindow_test);
