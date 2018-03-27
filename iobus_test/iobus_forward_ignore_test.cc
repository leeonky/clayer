#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_forward_ignore_test");

BEFORE_EACH() {
	init_subject("");
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("forward_last") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1\n");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_EQ(iob.forward_last(), -1);
	iob.get([](const char *, const char *){return 0;});

	CUE_ASSERT_EQ(iob.forward_last(), 0);
	CUE_ASSERT_STDOUT_EQ("TEST a:1\n");

	CUE_ASSERT_EQ(iob.forward_last(), -1);
}

SUITE_CASE("forward_last with no args") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST\n");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_EQ(iob.forward_last(), -1);
	iob.get([](const char *, const char *){return 0;});

	CUE_ASSERT_EQ(iob.forward_last(), 0);
	CUE_ASSERT_STDOUT_EQ("TEST\n");

	CUE_ASSERT_EQ(iob.forward_last(), -1);
}

SUITE_CASE("ignore_last") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_EQ(iob.ignore_last(), -1);
	iob.get([](const char *, const char *){return 0;});

	CUE_ASSERT_EQ(iob.ignore_last(), 0);
	CUE_ASSERT_STDOUT_EQ("");

	CUE_ASSERT_EQ(iob.ignore_last(), -1);
}

SUITE_END(iobus_forward_ignore_test);


