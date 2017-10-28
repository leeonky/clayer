#include <execinfo.h>
#include "stdexd.h"

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
