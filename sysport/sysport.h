#ifndef SYSPORT_SYSPORT_H
#define SYSPORT_SYSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

sem_t *sem_new_with_ppid(int, int);
sem_t *sem_load_with_ppid(int);
int sem_unlink_with_ppid(int);

#ifdef __cplusplus
}
#endif

#endif
