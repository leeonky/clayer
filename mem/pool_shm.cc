#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <cstdlib>
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "sysport/sysport.h"
#include "mem/pool_shm.h"

int pool_shm::create(size_t size, const std::function<int(pool_shm &)> &action) {
	return shmget(size, [&](int id) {
			last_shm_id = id;
			return shmat(id, [&](void *buf) {
				pool_shm shm(id, static_cast<int8_t *>(buf)); 
				register_ipc_clear_job();
				return action(shm);
			});
		});
}

const char *pool_shm::serialize_to_string() {
	static char buf[1024] = {};
	sprintf(buf, "POOL id:%d", shm_id);
	return buf;
}

int pool_shm::load(int shm_id, const std::function<int(pool_shm &)> &action) {
	return shmat(shm_id, [&](void *buf) {
			pool_shm shm(shm_id, static_cast<int8_t *>(buf));
			return action(shm);
			});
}
