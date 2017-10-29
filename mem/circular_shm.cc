#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
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

int circular_shm::initialize_and_action(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	element_size = alignment_size(size);
	element_count = count;
	return shmget(element_size*count, [this, &action](int id) {
			last_shm_id = shm_id = id;
			return shmat(id, [this, &action](void *buf) {
				buffer = static_cast<int8_t *>(buf);
				last_sem_id = sem_id = getpid();
				return sem_new_with_id(sem_id, element_count, [this, &action](sem_t *s){
					semaphore = s;
					register_cbuf_clean();
					return action(*this);
					});
				});
			});
}

int circular_shm::create(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	return circular_shm().initialize_and_action(size, count, action);
}

const char *circular_shm::serialize_to_string() {
	static char buf[1024] = {};
	sprintf(buf, "BUFFER id:%d size:%zd count:%d sem:%d", shm_id, element_size, element_count, sem_id);
	return buf;
}

