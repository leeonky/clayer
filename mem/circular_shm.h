#ifndef MEM_CIRCULAR_SHM_
#define MEM_CIRCULAR_SHM_

#include <functional>
#include <semaphore.h>

class circular_shm {

public:
	static int create(size_t, int, const std::function<int(circular_shm &)> &);
	const char *serialize_to_string();

	void *allocate() {
		if(sem_wait(semaphore) == -1)
			perror("semaphore wait error");
		index = (index+1)%element_count;
		return buffer+element_size*index;
	}
	size_t element_size;
	int element_count;
	unsigned index = 0;

private:
	int shm_id, sem_id;
	int8_t *buffer;
	sem_t *semaphore;

	circular_shm() {}
	int initialize_and_action(size_t, int, const std::function<int(circular_shm &)> &);
};

#endif
