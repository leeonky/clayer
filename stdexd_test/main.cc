#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(stdarg_test);
	return run_test();
}

