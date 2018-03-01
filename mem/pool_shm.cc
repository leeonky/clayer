#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <cstdlib>
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"
#include "sysport/sysport.h"
#include "mem/pool_shm.h"

namespace {
	int last_shm_id = -1;
	static void clean_last_cbuf(int s) {
		if(-1 != last_shm_id)
			shmctl(last_shm_id, IPC_RMID, NULL);
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

int pool_shm::create(size_t size, const std::function<int(pool_shm &)> &action) {
	return shmget(size, [&](int id) {
			last_shm_id = id;
			return shmat(id, [&](void *buf) {
				pool_shm shm(id, static_cast<int8_t *>(buf)); 
				register_cbuf_clean();
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
