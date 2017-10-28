#include <stdio.h>
#include <stdlib.h>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

int main() {
	init_test();
	ADD_SUITE(circular_shm_alloc_test);
	return run_test();
}
