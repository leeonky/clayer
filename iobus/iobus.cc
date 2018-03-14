#include <cstring>
#include <cstdarg>
#include "stdexd/stdexd.h"
#include "iobus.h"

#undef log_error
#define log_error(format, ...) log_error("libiobus", (format), ## __VA_ARGS__)

void iobus::post(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(file_out, format, args);
	fprintf(file_out, "\n");
	fflush(file_out);
	va_end(args);
}

int iobus::get(const std::function<int(const char *, const char *)> &action) {
	size_t len = 0;
	if(processed) {
		if(-1 != getline(&line, &len, file_in) && 1 == sscanf(line, "%s", command)) {
			const char *p = index(line, ' ');
			arguments = p ? p+1 : "";
			processed = false;
		} else
			return -1;
	}
	return action(command, arguments);
}

int iobus::get(const char *event, const std::function<int(void)> &action, int arg_count, const char *format, ...) {
	int res = 0;
	va_list args;
	va_start(args, format);
	res = get([&](const char *command, const char *arguments) {
			int r = 0;
			if(!strcmp(command, event)) {
				accept_processed();
				if(arg_count == vsscanf(arguments, format, args))
					r = action();
				else
					r = log_error("Invalid %s arguments '%s'", command, arguments);
			} else
				r = -1;
			return r;
			});
	va_end(args);
	return res;
}

int iobus::get(const char *event, const std::function<int(const char *)> &action) {
	int res = 0;
	res = get([&](const char *command, const char *arguments) {
			int r = 0;
			if(!strcmp(command, event)) {
				accept_processed();
				r = action(arguments);
			} else
				r = -1;
			return r;
			});
	return res;
}

int iobus::pass_through() {
	int ret = get([&](const char *, const char *){
			return forward_last();
			});
	return ret;
}

int iobus::except(const char *command) {
	int ret = get([&](const char *cmd, const char *){
			return !strcmp(cmd, command);
			});
	return ret;
}

