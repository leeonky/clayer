#include <sys/shm.h>
#include <cstdio>
#include <cstring>
#include "stdexd/stdexd.h"
#include "sysexd.h"
#include "sysport/sysport.h"

namespace {
	static void output_errno() {
		fprintf(app_stderr, "Error[shm_cbuf]: %s\n", strerror(errno));
		print_stack(app_stderr);
	}
}

int shmget(size_t size, const std::function<int(int)> &action) {
	int res, id;
	if((id = shmget(IPC_PRIVATE, size, 0666|IPC_CREAT)) != -1) {
		res = action(id);
		shmctl(id, IPC_RMID, nullptr);
	} else {
		res = -1;
		output_errno();
	}
	return res;
}

int shmat(int id, const std::function<int(void *)> &action) {
	int res;
	void *buffer;
	if((buffer = static_cast<int8_t*>(shmat(id, nullptr, 0))) != (void *)-1) {
		res = action(buffer);
		shmdt(buffer);
	} else {
		res = -1;
		output_errno();
	}
	return res;
}

int sem_new_with_id(int id, int count, const std::function<int(sem_t *)> &action) {
	int res;
	sem_t *semaphore;
	if(SEM_FAILED != (semaphore = sem_new_with_id(id, count))) {
		res = action(semaphore);
		sem_close(semaphore);
		sem_unlink_with_id(id);
	} else {
		res = -1;
		output_errno();
	}
	return res;
}

