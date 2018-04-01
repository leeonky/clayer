#ifndef SYSEXD_SYSEXD_H
#define SYSEXD_SYSEXD_H

#include <functional>
#include <semaphore.h>
#include <sys/ipc.h>
#include <cerrno>
#include <cstring>

int shmget(size_t, const std::function<int(int)> &);

int shmat(int, const std::function<int(void *)> &);

int sem_new_with_id(int, int, const std::function<int(sem_t *)> &);

int sem_load_with_id(int, const std::function<int(sem_t *)> &);

int msgget(const std::function<int(int)> &);

template<typename Arg>
int msgsnd(int msgid, const Arg &arg, const std::function<int(void)> &action) {
	int res = 0;
	if(!(res = msgsnd(msgid, &arg, sizeof(Arg)-sizeof(long), IPC_NOWAIT)))
		res = action();
	else
		res = log_errno("libsysexd", errno, strerror_r);
	return res;
}

int msgsnd(int, const char *, const std::function<int(void)> &);

int msgrcv(int, const std::function<int(const char *)> &);

extern int last_shm_id, last_sem_id, last_msg_id;
extern void register_ipc_clear_job();

#endif

