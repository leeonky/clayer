#include <cstring>
#include "iobus.h"

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

