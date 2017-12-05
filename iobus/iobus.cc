#include <cstring>
#include <cstdarg>
#include "stdexd/stdexd.h"
#include "iobus.h"

#undef log_error
#define log_error(format, ...) log_error("libiobus", (format), ## __VA_ARGS__)

int iobus::get(const std::function<int(const char *, const char *)> &action) {
	int res = 0;
	size_t len = 0;
	if(processed) {
		if(-1 != getline(&line, &len, file_in) && 1 == sscanf(line, "%s", command)) {
			const char *p = index(line, ' ');
			arguments = p ? p+1 : "";
			processed = false;
		} else
			return -1;
	}
	if(!(res = action(command, arguments)))
		processed = true;
	return res;
}

int iobus::get(const char *event, const std::function<int(const char *, const char *)> &action, int arg_count, const char *format, ...) {
	int res = 0;
	va_list args;
	va_start(args, format);
	res = get([&](const char *command, const char *arguments) {
			int r = 0;
			if(!strcmp(command, event)) {
				if(arg_count == vsscanf(arguments, format, args)) {
					r = action(command, arguments);
				} else
					r = log_error("Invalid %s arguments '%s'", command, arguments);
			} else
				r = -1;
			return r;
			});
	va_end(args);
	return res;
}

