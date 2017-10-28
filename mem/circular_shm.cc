#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include "circular_shm.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
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
	//int res = 0;
	element_size = alignment_size(size);
	return shmget(element_size*count, [action](int id) {
			return 0;
			});

	//if((shm_id = shmget(IPC_PRIVATE, count*element_size, 0666|IPC_CREAT)) != -1) {
		//if((buffer = static_cast<int8_t*>(shmat(shm_id, nullptr, 0))) != (void *)-1) {
			//sem_id = getpid();
			//if(SEM_FAILED != (semaphore = sem_new_with_id(sem_id, count))) {
				//res = action(*this);
				//sem_close(semaphore);
				//sem_unlink_with_id(sem_id);
			//} else {
				//res = -1;
				//output_errno();
			//}
			//shmdt(buffer);
		//} else {
			//res = -1;
			//output_errno();
		//}
		//shmctl(shm_id, IPC_RMID, nullptr);
	//} else {
		//res = -1;
		//output_errno();
	//}
	//return res;
}

int circular_shm::alloc(size_t size, int count, const std::function<int(circular_shm &)> &action) {
	return circular_shm().initialize_and_action(size, count, action);
}

