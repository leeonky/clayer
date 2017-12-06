#include <cunitexd.h>

#ifdef __APPLE__
int strerror_r(int e, char *buffer, size_t buflen) {
	snprintf(buffer, buflen, "%d", e);
	return 0;
}
#else
char *strerror_r(int e, char *buffer, size_t buflen) {
	snprintf(buffer, buflen, "%d", e);
	return buffer;
}
#endif

int main() {
	init_test();
	ADD_SUITE(stdarg_test);
	ADD_SUITE(fmemopen_test);
	return run_test();
}

