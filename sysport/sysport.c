#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>

static const char *sem_name(int id) {
	        static __thread char buffer[128];
		        sprintf(buffer, "/%d", id);
			        return buffer;
}

sem_t *sem_new_with_ppid(int id, int value) {
	        return sem_open(sem_name(id), O_CREAT|O_EXCL, 0644, value);
}

sem_t *sem_load_with_ppid(int id) {
	        return sem_open(sem_name(id), 0);
}

int sem_unlink_with_ppid(int id) {
	        return sem_unlink(sem_name(id));
}

