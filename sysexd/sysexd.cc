#include <sys/shm.h>
#include <cstdio>
#include <cstring>
#include "stdexd/stdexd.h"
#include "sysexd.h"
#include "sysport/sysport.h"

#undef log_error
#define log_error(format, ...) log_error("libsysexd", (format), ## __VA_ARGS__)

#undef log_errno
#define log_errno() log_errno("libsysexd", errno, strerror_r)

int shmget(size_t size, const std::function<int(int)> &action) {
	int res, id;
	if((id = shmget(IPC_PRIVATE, size, 0666|IPC_CREAT)) != -1) {
		res = action(id);
		shmctl(id, IPC_RMID, nullptr);
	} else {
		res = log_errno();
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
		res = log_errno();
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
		res = log_errno();
	}
	return res;
}
