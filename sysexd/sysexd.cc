#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
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
	} else
		res = log_errno();
	return res;
}

int sem_load_with_id(int id, const std::function<int(sem_t *)> &action) {
	int res;
	sem_t *semaphore;
	if(SEM_FAILED != (semaphore = sem_load_with_id(id))) {
		res = action(semaphore);
		sem_close(semaphore);
	} else
		res = log_errno();
	return res;
}

int msgget(const std::function<int(int)> &action) {
	int res = 0;
	int id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	if(id != -1) {
		last_msg_id = id;
		register_ipc_clear_job();
		res = action(id);
		msgctl(id, IPC_RMID, nullptr);
	} else
		res = log_errno();
	return res;
}

int msgsnd(int msgid, const char *message, const std::function<int(void)> &action) {
	int res = 0;
	size_t msg_len = strlen(message)+1;
	char *buffer = new char[msg_len+sizeof(long)];
	*(long *)buffer = 1;
	strcpy(buffer+sizeof(long), message);
	if(!(res = msgsnd(msgid, buffer, msg_len, IPC_NOWAIT)))
		res = action();
	else
		res = log_errno();
	delete[] buffer;
	return res;
}

int msgrcv(int msgid, const std::function<int(const char *)> &action) {
	char buffer[1024];
	int res = 0;
	ssize_t ret = msgrcv(msgid, buffer, sizeof(buffer), 1, IPC_NOWAIT);
	if(ret != -1)
		res = action(buffer+sizeof(long));
	else
		res = log_errno();
	return res;
}

int last_shm_id=-1, last_sem_id=-1, last_msg_id=-1;

namespace {
	void clean_last_cbuf(int) {
		if(-1 != last_shm_id)
			shmctl(last_shm_id, IPC_RMID, NULL);
		if(-1 != last_sem_id)
			sem_unlink_with_id(last_sem_id);
		if(-1 != last_msg_id)
			msgctl(last_msg_id, IPC_RMID, nullptr);
		exit(-1);
	}
}

void register_ipc_clear_job() {
	signal(SIGINT, clean_last_cbuf);
	signal(SIGHUP, clean_last_cbuf);
	signal(SIGKILL, clean_last_cbuf);
	signal(SIGPIPE, clean_last_cbuf);
	signal(SIGTERM, clean_last_cbuf);
}
