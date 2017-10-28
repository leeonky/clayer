#ifndef MOCK_SYSCALL_
#define MOCK_SYSCALL_

#include <cunitexd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
