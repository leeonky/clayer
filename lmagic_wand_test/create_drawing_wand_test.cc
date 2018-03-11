#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lmagic_wand/lmagic_wand.h"
#include "mock_magic_wand/mock_magic_wand.h"

SUITE_START("create_drawing_wand_test");

static DrawingWand *ret_dwand;

mock_function_1(int, drawing_wand_action, DrawingWand *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_dwand = (DrawingWand *)&ret_dwand;

	init_mock_function_with_return(NewDrawingWand, ret_dwand);
	init_mock_function(DrawSetGravity);
	init_mock_function(DrawSetTextEncoding);
	init_mock_function(drawing_wand_action);
	init_mock_function(DestroyDrawingWand);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return NewDrawingWand(drawing_wand_action);
}

SUITE_CASE("create drawing wand") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(NewDrawingWand);

	CUE_EXPECT_CALLED_ONCE(DrawSetGravity);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetGravity, 1, ret_dwand);
	CUE_EXPECT_CALLED_WITH_INT(DrawSetGravity, 2, NorthWestGravity);

	CUE_EXPECT_CALLED_ONCE(DrawSetTextEncoding);
	CUE_EXPECT_CALLED_WITH_PTR(DrawSetTextEncoding, 1, ret_dwand);
	CUE_EXPECT_CALLED_WITH_STRING(DrawSetTextEncoding, 2, "UTF8");

	CUE_EXPECT_CALLED_ONCE(drawing_wand_action);
	CUE_EXPECT_CALLED_WITH_PTR(drawing_wand_action, 1, ret_dwand);

	CUE_EXPECT_CALLED_ONCE(DestroyDrawingWand);
	CUE_EXPECT_CALLED_WITH_PTR(DestroyDrawingWand, 1, ret_dwand);
}

SUITE_CASE("new drawing wand failed") {
	init_mock_function_with_return(NewDrawingWand, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblmagic_wand]: DrawingWand error\n");

	CUE_EXPECT_NEVER_CALLED(DrawSetGravity);

	CUE_EXPECT_NEVER_CALLED(DrawSetTextEncoding);

	CUE_EXPECT_NEVER_CALLED(drawing_wand_action);

	CUE_EXPECT_NEVER_CALLED(DestroyDrawingWand);
}

SUITE_END(create_drawing_wand_test);

