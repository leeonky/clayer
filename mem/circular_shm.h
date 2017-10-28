#ifndef MEM_CIRCULAR_SHM_
#define MEM_CIRCULAR_SHM_

#include <functional>
#include <semaphore.h>

class circular_shm {
public:
	static int alloc(size_t, int, const std::function<int(circular_shm &)> &);

private:
	size_t element_size;
	int count, shm_id, sem_id;
	int8_t *buffer;
	sem_t *semaphore;

	int initialize_and_action(size_t, int, const std::function<int(circular_shm &)> &);
};

#endif
