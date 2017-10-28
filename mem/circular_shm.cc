#include <sys/shm.h>
#include <unistd.h>
#include "circular_shm.h"

namespace {
	inline size_t alignment_size(size_t size) {
		size_t page_size = getpagesize();
		return (size+page_size-1)/page_size*page_size;
	}
}

int circular_shm::init(size_t size, int count) {
	int res = 0;
	if((shm_id = shmget(IPC_PRIVATE, element_size = count*alignment_size(size), 0666|IPC_CREAT)) != -1) {
		buffer = static_cast<int8_t*>(shmat(shm_id, nullptr, 0));
	} else
		res = -1;
	return res;
}

void circular_shm::destroy() {
	shmdt(buffer);
	shmctl(shm_id, IPC_RMID, nullptr);
}

int circular_shm::alloc(size_t size, int count, std::function<int(circular_shm &)> action) {
	int res;
	circular_shm shm;
	if(!(res = shm.init(size, count))) {
		res = action(shm);
		shm.destroy();
	}
	return res;
}

