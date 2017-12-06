#include "mock_std.h"

mock_function_3(FILE *, fmemopen, void *, size_t, const char *);
mock_function_2(FILE *, fopen, const char *, const char *);
mock_function_1(int, fclose, FILE *);
