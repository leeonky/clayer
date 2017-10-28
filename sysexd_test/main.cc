#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(shmget_test);
	ADD_SUITE(shmat_test);
	ADD_SUITE(sem_new_with_id_test);
	return run_test();
}

