#ifndef STDPORT_STDPORT_H
#define STDPORT_STDPORT_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef __APPLE__
#include <stdio.h>

FILE *fmemopen(void *, size_t, const char *);

#endif

int64_t usectime();

#ifdef __cplusplus
}
#endif

#endif
