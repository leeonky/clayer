#ifndef MEM_POOL_SHM_
#define MEM_POOL_SHM_

#include <functional>
#include <semaphore.h>

class pool_shm {

public:
	static int create(size_t, const std::function<int(pool_shm &)> &);
	static int load(int, const std::function<int(pool_shm &)> &);

	const char *serialize_to_string();

	void *allocate(size_t size, int align=64) {
		int8_t *ret = allocated;
		allocated += (size+align-1)/align*align;
		return ret;
	}

	size_t offset_of(const void *addr) const {
		return static_cast<const int8_t *>(addr)-buffer;
	}

	void *addr_by(size_t offset) const {
		return buffer + offset;
	}

private:
	int shm_id;
	int8_t *buffer, *allocated;

	pool_shm(int sid, int8_t *buf)
		: shm_id(sid), buffer(buf), allocated(buf) {}
};

#endif
