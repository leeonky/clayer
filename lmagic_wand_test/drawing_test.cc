#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lmagic_wand/lmagic_wand.h"
#include "mock_magic_wand/mock_magic_wand.h"

SUITE_START("drawing_test");

static MagickWand *arg_mwand;
static DrawingWand *arg_dwand;
static PixelWand *ret_pwand;

static int arg_font_size;
static const char *arg_font_file, *arg_font_color;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_font_file = "font";
	arg_font_size = 100;
	arg_font_color = "red";

	arg_dwand = (DrawingWand *)&arg_dwand;
	arg_mwand = (MagickWand *)&arg_mwand;
	ret_pwand = (PixelWand *)&ret_pwand;

	init_mock_function_with_return(DrawSetFont, MagickTrue);
	init_mock_function(DrawSetFontSize);

	init_mock_function_with_return(NewPixelWand, ret_pwand);
	init_mock_function_with_return(PixelSetColor, MagickTrue);
	init_mock_function(DrawSetFillColor);
	init_mock_function(DestroyPixelWand);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("set font size and font file") {
	CUE_ASSERT_EQ(DrawSetFont(arg_dwand, arg_font_size, arg_font_file), 0);

	CUE_EXPECT_CALLED_ONCE(DrawSetFont);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetFont, 1, arg_dwand);
	CUE_EXPECT_CALLED_WITH_STRING(DrawSetFont, 2, arg_font_file);

	CUE_EXPECT_CALLED_ONCE(DrawSetFontSize);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetFontSize, 1, arg_dwand);
	CUE_EXPECT_CALLED_WITH_INT(DrawSetFontSize, 2, arg_font_size);
}

SUITE_CASE("failed to set font") {
	init_mock_function_with_return(DrawSetFont, MagickFalse);

	CUE_ASSERT_EQ(DrawSetFont(arg_dwand, arg_font_size, arg_font_file), -1);

	CUE_ASSERT_STDERR_EQ("Error[liblmagic_wand]: DrawingWand error\n");

	CUE_EXPECT_NEVER_CALLED(DrawSetFontSize);
}

SUITE_CASE("DrawSetFillColor") {
	DrawSetFillColor(arg_dwand, arg_font_color);

	CUE_EXPECT_CALLED_ONCE(NewPixelWand);

	CUE_EXPECT_CALLED_ONCE(PixelSetColor);
	CUE_EXPECT_CALLED_WITH_PTR(PixelSetColor, 1, ret_pwand);
	CUE_EXPECT_CALLED_WITH_STRING(PixelSetColor, 2, arg_font_color);

	CUE_EXPECT_CALLED_ONCE(DrawSetFillColor);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetFillColor, 1, arg_dwand);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetFillColor, 2, ret_pwand);

	CUE_EXPECT_CALLED_ONCE(DestroyPixelWand);
	CUE_EXPECT_CALLED_WITH_PTR(DestroyPixelWand, 1, ret_pwand);
}

SUITE_END(drawing_test);

