#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(iobus_post_test);
	ADD_SUITE(iobus_get_test);
	ADD_SUITE(iobus_forward_ignore_test);
	ADD_SUITE(iobus_ignore_untill_test);
	ADD_SUITE(iobus_forward_untill_test);
	return run_test();
}

