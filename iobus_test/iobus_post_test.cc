#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_post_test");

BEFORE_EACH() {
	init_subject("");
	app_stdout = actxt.output_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("post message") {
	iobus iob(app_stdin, app_stdout, app_stderr);

	iob.post("hello");

	CUE_ASSERT_STDOUT_EQ("hello\n");
}

SUITE_CASE("post last") {
	iobus iob(app_stdin, app_stdout, app_stderr);

	iob.recaption_and_post();

	CUE_ASSERT_STDOUT_EQ("");
}

SUITE_CASE("post last with content") {
	init_subject("HELLO WORLD\n");
	app_stdout = actxt.output_stream;
	app_stdin = actxt.input_stream;

	iobus iob(app_stdin, app_stdout, app_stderr);

	iob.get([&](const char *, const char *){
			iob.recaption_and_post();
			return 0;
			});

	CUE_ASSERT_STDOUT_EQ("HELLO WORLD\n");
}

SUITE_END(iobus_post_test);

