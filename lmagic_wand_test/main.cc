#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(create_magic_wand_test);
	return run_test();
}

