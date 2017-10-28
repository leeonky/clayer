#ifndef MEM_CIRCULAR_SHM_
#define MEM_CIRCULAR_SHM_

#include <functional>

class circular_shm {
public:
	static int alloc(size_t, int, std::function<int(circular_shm &)>);

private:
	size_t element_size;
	int count, shm_id;
	int8_t *buffer;

	int init(size_t, int);
	void destroy();
};

#endif
