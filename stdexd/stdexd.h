#ifndef STDEXD_STDEXD_H
#define STDEXD_STDEXD_H

#include <functional>
#include <vector>
#include <map>
#include <utility>
#include <cstdarg>
#include <cstdio>
#include <getopt.h>

extern FILE *app_stdin, *app_stdout, *app_stderr;

void print_stack(FILE *);

extern int log_error(const char *, const char *, ...);
extern int log_warning(const char *, const char *, ...);

template<typename Ter, typename Estr>
int log_errno(const char *module, Ter er, Estr ster) {
	char buffer[1024];
	ster(er, buffer, sizeof(buffer));
	return log_error(module, "%s", buffer);
}

class command_argument {

public:
	command_argument require_full_argument(const char *, int, const std::function<void(const char *)> &);
	const char *parse(int, char **);

private:
	std::vector<option> long_options;
	std::map<int, std::function<void(const char *)>> argument_handlers;
};

int fmemopen(void *, size_t, const char *, const std::function<int(FILE *)> &);
int fopen(const char *, const char *, const std::function<int(FILE *)> &);

#endif
