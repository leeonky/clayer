#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"
#include "mock_sys/mock_sys.h"

SUITE_START("media_clock_test");

static int64_t ret_usec;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_usec = 123456;
	init_mock_function_with_return(usectime, ret_usec);
	init_mock_function(usleep);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("wait for present in right second") {
	init_mock_function_with_return(usectime, 3000);
	media_clock clock;
	init_mock_function_with_return(usectime, 3010);

	CUE_ASSERT_EQ(clock.wait(60, 100), 0);

	CUE_EXPECT_CALLED_ONCE(usleep);
	CUE_EXPECT_CALLED_WITH_INT(usleep, 1, 50);
}

//SUITE_CASE("do not wait when time is synced") {
	//media_clock mclk = {3000, 20};
	//arg_usec = 3040;

	//CUE_ASSERT_EQ(mclk_waiting(&mclk, 60, 100), 0);

	//CUE_EXPECT_NEVER_CALLED(usleep);
//}

//SUITE_CASE("failed and do not wait if time passed") {
	//media_clock mclk = {3000, 20};
	//arg_usec = 3041;

	//CUE_ASSERT_EQ(mclk_waiting(&mclk, 60, 10), -1);

	//CUE_EXPECT_NEVER_CALLED(usleep);
//}

//SUITE_CASE("if too long time to wait, just wait period") {
	//media_clock mclk = {3000, 20};
	//arg_usec = 3000;

	//CUE_ASSERT_EQ(mclk_waiting(&mclk, 60, 39), 0);

	//CUE_EXPECT_CALLED_ONCE(usleep);
	//CUE_EXPECT_CALLED_WITH_INT(usleep, 1, 39);
//}

//SUITE_CASE("sync clock") {
	//media_clock mclk = {};

	//mclk_sync(&mclk, 1000, 200);

	//CUE_ASSERT_EQ(mclk.base, 1000);
	//CUE_ASSERT_EQ(mclk.offset, 200);
//}

SUITE_END(media_clock_test);

