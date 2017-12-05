#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(stdarg_test);
	ADD_SUITE(fmemopen_test);
	return run_test();
}

