#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(iobus_post_test);
	ADD_SUITE(iobus_get_test);
	return run_test();
}

