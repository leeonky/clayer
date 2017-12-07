#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <cstdlib>
#include "circular_shm.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "sysport/sysport.h"
#include "mem/circular_shm.h"

namespace {
	inline size_t alignment_size(size_t size) {
		size_t page_size = getpagesize();
		return (size+page_size-1)/page_size*page_size;
	}

	int last_shm_id = -1, last_sem_id = -1;
	static void clean_last_cbuf(int s) {
		if(-1 != last_shm_id)
			shmctl(last_shm_id, IPC_RMID, NULL);
		if(-1 != last_sem_id)
			sem_unlink_with_id(last_sem_id);
		fprintf(stderr, "abort(%d)\n", s);
		exit(-1);
	}

	void register_cbuf_clean() {
		signal(SIGINT, clean_last_cbuf);
		signal(SIGHUP, clean_last_cbuf);
		signal(SIGKILL, clean_last_cbuf);
		signal(SIGPIPE, clean_last_cbuf);
		signal(SIGTERM, clean_last_cbuf);
	}
}

int circular_shm::create(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	size_t element_size = alignment_size(size);
	return shmget(element_size*count, [&](int id) {
			last_shm_id = id;
			return shmat(id, [&](void *buf) {
				int sem_id = getpid();
				last_sem_id = sem_id;
				return sem_new_with_id(sem_id, count, [&](sem_t *s){
					circular_shm shm(id, element_size, count, sem_id, static_cast<int8_t *>(buf), s);
					register_cbuf_clean();
					return action(shm);
					});
				});
			});
}

int circular_shm::load(int shm_id, size_t element_size, int count, int sem_id, const std::function<int(circular_shm &)> &action) {
	return shmat(shm_id, [&](void *buf) {
			return sem_load_with_id(sem_id, [&](sem_t *s) {
				circular_shm shm(shm_id, element_size, count, sem_id, static_cast<int8_t *>(buf), s);
				return action(shm);
				});
			});
}

const char *circular_shm::serialize_to_string() {
	static char buf[1024] = {};
	sprintf(buf, "BUFFER id:%d size:%zd count:%d sem:%d", shm_id, element_size, element_count, sem_id);
	return buf;
}

