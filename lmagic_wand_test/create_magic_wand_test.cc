#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lmagic_wand/lmagic_wand.h"
#include "mock_magic_wand/mock_magic_wand.h"

SUITE_START("create_magic_wand_test");

static MagickWand *ret_mwand;
static int arg_width, arg_height;
static const char *arg_file;

mock_function_1(int, magick_wand_action, MagickWand *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_mwand = (MagickWand *)&ret_mwand;

	arg_width = 1920; arg_height = 1080;
	arg_file = "input";

	init_mock_function_with_return(NewMagickWand, ret_mwand);
	init_mock_function_with_return(MagickSetSize, MagickTrue);
	init_mock_function_with_return(MagickReadImage, MagickTrue);
	init_mock_function(magick_wand_action);
	init_mock_function(DestroyMagickWand);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return NewMagickWand(arg_width, arg_height, arg_file, magick_wand_action);
}

SUITE_CASE("create magic wand with size and file") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(NewMagickWand);

	CUE_EXPECT_CALLED_ONCE(MagickSetSize);
	CUE_EXPECT_CALLED_WITH_PTR(MagickSetSize, 1, ret_mwand);
	CUE_EXPECT_CALLED_WITH_INT(MagickSetSize, 2, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(MagickSetSize, 3, arg_height);

	CUE_EXPECT_CALLED_ONCE(MagickReadImage);
	CUE_EXPECT_CALLED_WITH_PTR(MagickReadImage, 1, ret_mwand);
	CUE_EXPECT_CALLED_WITH_STRING(MagickReadImage, 2, arg_file);

	CUE_EXPECT_CALLED_ONCE(magick_wand_action);
	CUE_EXPECT_CALLED_WITH_PTR(magick_wand_action, 1, ret_mwand);

	CUE_EXPECT_CALLED_ONCE(DestroyMagickWand);
	CUE_EXPECT_CALLED_WITH_PTR(DestroyMagickWand, 1, ret_mwand);
}

SUITE_CASE("NewMagickWand failed") {
	init_mock_function_with_return(NewMagickWand, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblmagic_wand]: MagickWand error\n");

	CUE_EXPECT_NEVER_CALLED(MagickSetSize);

	CUE_EXPECT_NEVER_CALLED(MagickReadImage);

	CUE_EXPECT_NEVER_CALLED(magick_wand_action);

	CUE_EXPECT_NEVER_CALLED(DestroyMagickWand);
}

SUITE_CASE("MagickSetSize failed") {
	init_mock_function_with_return(MagickSetSize, MagickFalse);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblmagic_wand]: MagickWand error\n");
}

SUITE_END(create_magic_wand_test);

