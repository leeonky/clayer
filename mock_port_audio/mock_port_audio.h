#ifndef MOCK_PORT_AUDIO_
#define MOCK_PORT_AUDIO_

#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <portaudio.h>

extern_mock_function_0(PaError, Pa_Initialize);
extern_mock_function_0(PaError, Pa_Terminate);

extern_mock_function_8(PaError, Pa_OpenStream, PaStream **, const PaStreamParameters *, const PaStreamParameters *, double, unsigned long, PaStreamFlags, PaStreamCallback *, void *);
extern_mock_function_1(PaError, Pa_CloseStream, PaStream *);

extern_mock_function_1(const PaDeviceInfo *, Pa_GetDeviceInfo, PaDeviceIndex);

#ifdef __cplusplus
}
#endif

#endif
