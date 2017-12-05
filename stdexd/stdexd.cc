#include <execinfo.h>
#include <numeric>
#include <string>
#include <algorithm>
#include "stdexd.h"

using std::string;

FILE *app_stdin = stdin, *app_stdout = stdout, *app_stderr = stderr;

#define MAX_STACK_DEPTH 100

void print_stack(FILE *f) {
	int fd = fileno(f);
	void *buffer[MAX_STACK_DEPTH];
	int depth;

	depth = backtrace(buffer, MAX_STACK_DEPTH);
	backtrace_symbols_fd(buffer, depth, fd);
}

int log_error(const char *module, const char *format, ...) {
	va_list list;
	fprintf(app_stderr, "Error[%s]: ", module);
	va_start(list, format);
	vfprintf(app_stderr, format, list);
	va_end(list);
	fprintf(app_stderr, "\n");
	print_stack(app_stderr);
	return -1;
}

command_argument command_argument::require_full_argument(const char *long_opt, int opt, const std::function<void(const char *)> &action) {
	command_argument ca(*this);
	ca.long_options.push_back({long_opt, required_argument, nullptr, opt});
	ca.argument_handlers.insert({opt, action});
	return ca;
}

const char *command_argument::parse(int argc, char **argv) {
	long_options.push_back({0, 0, 0, 0});
	int option_index, c;
	optind = 1;

	string option_str;
	std::for_each(argument_handlers.begin(), argument_handlers.end(),
			[&option_str](const std::pair<int, std::function<void(const char *)>> &handler){
			option_str += handler.first;
			option_str += ':';
			});

	while((c=getopt_long(argc, argv, option_str.c_str(), long_options.data(), &option_index)) != -1) {
		auto v = argument_handlers.find(c);
		if(v != argument_handlers.end())
			v->second(optarg);
	}
	if(optind < argc)
		return argv[optind];
	return "";
}

int fmemopen(void *buf, size_t size, const char *mode, const std::function<int(FILE *)> &action) {
	int res = 0;
	FILE *f = fmemopen(buf, size, mode);
	if(f) {
		res = action(f);
		fclose(f);
	} else
		res = -1;
	return res;
}

