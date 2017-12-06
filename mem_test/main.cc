#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(circular_shm_create_test);
	ADD_SUITE(circular_shm_load_test);
	return run_test();
}
