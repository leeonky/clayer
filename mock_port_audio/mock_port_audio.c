#include "mock_port_audio.h"

mock_function_0(PaError, Pa_Initialize);
mock_function_0(PaError, Pa_Terminate);

mock_function_8(PaError, Pa_OpenStream, PaStream **, const PaStreamParameters *, const PaStreamParameters *, double, unsigned long, PaStreamFlags, PaStreamCallback *, void *);
mock_function_1(PaError, Pa_CloseStream, PaStream *);

mock_function_1(const PaDeviceInfo *, Pa_GetDeviceInfo, PaDeviceIndex);

const char *Pa_GetErrorText(PaError er) {
	return "port audio error";
}
