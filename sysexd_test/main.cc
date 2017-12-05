#include <cunitexd.h>

char *strerror_r(int e, char *buffer, size_t buflen) {
	snprintf(buffer, buflen, "%d", e);
	return buffer;
}

int main() {
	init_test();
	ADD_SUITE(shmget_test);
	ADD_SUITE(shmat_test);
	ADD_SUITE(sem_new_with_id_test);
	ADD_SUITE(sem_load_with_id_test);
	return run_test();
}

