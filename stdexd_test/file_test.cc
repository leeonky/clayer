#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock.h"

SUITE_START("fmemopen_test");

mock_function_1(int, file_action, FILE *);

static FILE ret_file;

static FILE * stub_fmemopen(void *, size_t, const char *) {
	return &ret_file;
}

BEFORE_EACH() {
	init_mock_function_with_function(fmemopen, stub_fmemopen);
	init_mock_function(fclose);
	init_mock_function(file_action);
	return 0;
}

SUITE_CASE("open read and close") {
	char buf[100];
	init_mock_function_with_return(file_action, 10);

	CUE_ASSERT_EQ(fmemopen(buf, 100, "r", file_action), 10);

	CUE_EXPECT_CALLED_ONCE(fmemopen);
	CUE_EXPECT_CALLED_WITH_PTR(fmemopen, 1, buf);
	CUE_EXPECT_CALLED_WITH_INT(fmemopen, 2, 100);
	CUE_EXPECT_CALLED_WITH_STRING(fmemopen, 3, "r");

	CUE_EXPECT_CALLED_ONCE(file_action);
	CUE_EXPECT_CALLED_WITH_PTR(file_action, 1, &ret_file);


	CUE_EXPECT_CALLED_ONCE(fclose);
	CUE_EXPECT_CALLED_WITH_PTR(fclose, 1, &ret_file);
}

SUITE_CASE("filed to open") {
	char buf[100];
	init_mock_function_with_function(fmemopen, NULL);

	CUE_ASSERT_EQ(fmemopen(buf, 100, "r", file_action), -1);

	CUE_EXPECT_NEVER_CALLED(file_action);

	CUE_EXPECT_NEVER_CALLED(fclose);
}

SUITE_END(fmemopen_test);

