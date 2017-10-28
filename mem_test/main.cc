#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(circular_shm_alloc_test);
	return run_test();
}
