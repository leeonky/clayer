#ifndef STDEXD_STDEXD_H
#define STDEXD_STDEXD_H

#include <cstdarg>
#include <cstdio>

extern FILE *app_stdin, *app_stdout, *app_stderr;

void print_stack(FILE *);

extern int log_error(const char *, const char *, ...);

template<typename Ter, typename Estr>
int log_errno(const char *module, Ter er, Estr ster) {
	char buffer[1024];
	ster(er, buffer, sizeof(buffer));
	return log_error(module, "%s", buffer);
}

#endif
