#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include <iostream>
#include <sstream> 
#include <string>
#include "stdexd/stdexd.h"
#include "media/sub_srt.h"

SUITE_START("sub_srt_test");

static std::string ret_title;
mock_void_function_1(subtitle_action, const std::string *);
void subtitle_action_ref(const std::string &title) {
	ret_title = title;
	subtitle_action(&title);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	init_mock_function(subtitle_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("select subtite by time") {
	init_subject(R"(1
12:34:56,789 --> 23:45:67,890
hello)");
	subtitle_srt ins(actxt.input_stream);

	int64_t from_time = (12*3600+34*60+56)*1000000L+789000;
	ins.query_item(from_time, subtitle_action_ref);

	CUE_EXPECT_CALLED_ONCE(subtitle_action);
	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "hello");
}

SUITE_CASE("select another subtite by time") {
	init_subject(R"(1
00:00:00,000 --> 00:00:00,001
t1

2
00:00:00,002 --> 00:00:00,003
t2

3
00:00:00,004 --> 00:00:00,005
t3
)");
	subtitle_srt ins(actxt.input_stream);

	ins.query_item(1000, subtitle_action_ref);
	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "t1\n");

	ins.query_item(4000, subtitle_action_ref);
	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "t3\n");

	ins.query_item(2000, subtitle_action_ref);
	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "t2\n");
}

SUITE_CASE("no subtitle hited") {
	init_subject(R"(1
00:00:00,000 --> 00:00:00,001
t1)");
	subtitle_srt ins(actxt.input_stream);

	ins.query_item(1000, subtitle_action_ref);
	ins.query_item(2000, subtitle_action_ref);

	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "");

	init_mock_function(subtitle_action);
	ins.query_item(2000, subtitle_action_ref);
	CUE_EXPECT_NEVER_CALLED(subtitle_action);
}

SUITE_CASE("query same subtitle") {
	init_subject(R"(1
00:00:00,001 --> 00:00:00,002
t1)");
	subtitle_srt ins(actxt.input_stream);

	ins.query_item(1000, subtitle_action_ref);
	ins.query_item(1000, subtitle_action_ref);

	CUE_EXPECT_CALLED_ONCE(subtitle_action);
	CUE_ASSERT_STRING_EQ(ret_title.c_str(), "t1");
}

SUITE_END(sub_srt_test);


