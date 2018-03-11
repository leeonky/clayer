#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(create_magic_wand_test);
	ADD_SUITE(create_drawing_wand_test);
	ADD_SUITE(drawing_test);
	return run_test();
}

