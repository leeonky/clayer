#ifndef MOCK_SYSCALL_
#define MOCK_SYSCALL_


#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <unistd.h>

extern_mock_function_3(int, shmget, key_t, size_t, int);
extern_mock_function_3(void *, shmat, int, const void *, int);
extern_mock_function_3(int, shmctl, int, int, struct shmid_ds *);
extern_mock_function_1(int, shmdt, const void *);

extern_mock_function_1(int, sem_wait, sem_t *);
extern_mock_function_1(int, sem_post, sem_t *);

extern_mock_function_2(sem_t *, sem_new_with_id, int, int);
extern_mock_function_1(sem_t *, sem_load_with_id, int);
extern_mock_function_1(int, sem_close, sem_t *);
extern_mock_function_1(int, sem_unlink_with_id, int);

extern_mock_function_0(int64_t, usectime);
extern_mock_function_1(int, usleep, useconds_t);

extern_mock_function_2(int, msgget, key_t, int);
extern_mock_function_3(int, msgctl, int, int, struct msqid_ds *);
extern_mock_function_4(int, msgsnd, int, const void *, size_t, int);
extern_mock_function_5(ssize_t, msgrcv, int, void *, size_t, long, int);

#ifdef __cplusplus
}
#endif

#endif
