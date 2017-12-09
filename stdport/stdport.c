#include "stdport.h"
#include <sys/time.h>
#ifdef __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

struct fmem {
	size_t pos;
	size_t size;
	char *buffer;
};
typedef struct fmem fmem_t;

static int readfn(void *handler, char *buf, int size) {
	fmem_t *mem = handler;
	size_t available = mem->size - mem->pos;

	if (size > available) {
		size = available;
	}
	memcpy(buf, mem->buffer + mem->pos, sizeof(char) * size);
	mem->pos += size;

	return size;
}

static int writefn(void *handler, const char *buf, int size) {
	fmem_t *mem = handler;
	size_t available = mem->size - mem->pos;

	if (size > available) {
		size = available;
	}
	memcpy(mem->buffer + mem->pos, buf, sizeof(char) * size);
	mem->pos += size;

	return size;
}

static fpos_t seekfn(void *handler, fpos_t offset, int whence) {
	size_t pos;
	fmem_t *mem = handler;

	switch (whence) {
		case SEEK_SET: {
				       if (offset >= 0) {
					       pos = (size_t)offset;
				       } else {
					       pos = 0;
				       }
				       break;
			       }
		case SEEK_CUR: {
				       if (offset >= 0 || (size_t)(-offset) <= mem->pos) {
					       pos = mem->pos + (size_t)offset;
				       } else {
					       pos = 0;
				       }
				       break;
			       }
		case SEEK_END: pos = mem->size + (size_t)offset; break;
		default: return -1;
	}

	if (pos > mem->size) {
		return -1;
	}

	mem->pos = pos;
	return (fpos_t)pos;
}

static int closefn(void *handler) {
	free(handler);
	return 0;
}

FILE *fmemopen(void *buf, size_t size, const char *mode) {
	fmem_t* mem = (fmem_t *) malloc(sizeof(fmem_t));

	memset(mem, 0, sizeof(fmem_t));

	mem->size = size;
	mem->buffer = buf;

	return funopen(mem, readfn, writefn, seekfn, closefn);
}

#endif

int64_t usectime() {
	int64_t us = 0;
	struct timeval tv;
	if(!gettimeofday(&tv, 0)) {
		us = tv.tv_sec;
		us = us*1000000 + tv.tv_usec;
	}
	return us;
}

