#ifndef SYSEXD_SYSEXD_H
#define SYSEXD_SYSEXD_H

#include <functional>
#include <semaphore.h>

int shmget(size_t, const std::function<int(int)> &);

int shmat(int, const std::function<int(void *)> &);

int sem_new_with_id(int, int, const std::function<int(sem_t *)> &);

int sem_load_with_id(int, const std::function<int(sem_t *)> &);

#endif

