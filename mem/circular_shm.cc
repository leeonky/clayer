#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include "circular_shm.h"
#include "sysport/sysport.h"
#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"

namespace {
	inline size_t alignment_size(size_t size) {
		size_t page_size = getpagesize();
		return (size+page_size-1)/page_size*page_size;
	}
	static void output_errno() {
		fprintf(app_stderr, "Error[shm_cbuf]: %s\n", strerror(errno));
		print_stack(app_stderr);
	}
}

int circular_shm::initialize_and_action(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	int res = 0;
	element_size = count*alignment_size(size);
	if((shm_id = shmget(IPC_PRIVATE, element_size, 0666|IPC_CREAT)) != -1) {
		buffer = static_cast<int8_t*>(shmat(shm_id, nullptr, 0));
		sem_id = getpid();
		semaphore = sem_new_with_ppid(sem_id, count);
		res = action(*this);
		shmdt(buffer);
		shmctl(shm_id, IPC_RMID, nullptr);
		sem_close(semaphore);
		sem_unlink_with_ppid(sem_id);
	} else {
		res = -1;
		output_errno();
	}
	return res;
}

int circular_shm::alloc(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	return circular_shm().initialize_and_action(size, count, action);
}

