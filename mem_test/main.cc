#include <cunitexd.h>

char *strerror_r(int e, char *buffer, size_t buflen) {
	snprintf(buffer, buflen, "%d", e);
	return buffer;
}

int main() {
	init_test();
	ADD_SUITE(circular_shm_create_test);
	return run_test();
}
