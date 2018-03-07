#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include <iostream>
#include <sstream> 
#include "stdexd/stdexd.h"
#include "media/sub_srt.h"

SUITE_START("sub_srt_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("create and find subtitle") {
	init_subject(R"(
1
12:34:56,789 --> 23:45:67,890
hello
world)");
	subtitle_srt ins(actxt.input_stream);

	int64_t from_time = (12*3600+34*60+56)*1000000L+789000;
	int64_t to_time = (23*3600+45*60+67)*1000000L+890000;
	CUE_ASSERT_EQ(ins.get_item(from_time)->index, 1);
	CUE_ASSERT_EQ(ins.get_item(from_time)->from, from_time);
	CUE_ASSERT_EQ(ins.get_item(from_time)->to, to_time);
	CUE_ASSERT_STRING_EQ(ins.get_item(from_time)->content.c_str(), "hello\nworld");
}

SUITE_CASE("select subtite by time") {
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
	CUE_ASSERT_EQ(ins.get_item(1000)->index, 1);
	CUE_ASSERT_EQ(ins.get_item(4000)->index, 3);
	CUE_ASSERT_EQ(ins.get_item(2000)->index, 2);
}

SUITE_END(sub_srt_test);


