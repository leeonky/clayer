#include <cstdio>
#include <execinfo.h>

FILE *app_stdin = stdin, *app_stdout = stdout, *app_stderr = stderr;

#define MAX_STACK_DEPTH 100

void print_stack(FILE *f) {
	int fd = fileno(f);
	void *buffer[MAX_STACK_DEPTH];
	int depth;

	depth = backtrace(buffer, MAX_STACK_DEPTH);
	backtrace_symbols_fd(buffer, depth, fd);
}
