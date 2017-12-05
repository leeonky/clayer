#ifndef MOCK_H
#define MOCK_H
#include <cunitexd.h>


#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

extern_mock_function_3(FILE *, fmemopen, void *, size_t, const char *);
extern_mock_function_2(FILE *, fopen, const char *, const char *);
extern_mock_function_1(int, fclose, FILE *);

#ifdef __cplusplus
}
#endif

#endif
