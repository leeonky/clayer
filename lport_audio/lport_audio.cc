#include <cstring>
#include "lport_audio/lport_audio.h"
#include "stdexd/stdexd.h"

#undef log_error
#define log_error(format, ...) log_error("liblportaudio", (format), ## __VA_ARGS__)

#undef log_warning
#define log_warning(format, ...) log_warning("liblportaudio", (format), ## __VA_ARGS__)

#undef log_errno
#define log_errno(no) log_errno("liblportaudio", no, Pa_GetErrorTextExd)

namespace {
	int Pa_GetErrorTextExd(PaError er, char* buffer, size_t len) {
		strncpy(buffer, Pa_GetErrorText(er), len);
		return 0;
	}
}

int Pa_Init_OpenOutputStream(PaDeviceIndex device, int rate, int channel, PaSampleFormat format, const std::function<int(PaStream *)> &action) {
	int ret = 0;
	PaError ret_er;
	if(paNoError == (ret_er = Pa_Initialize())) {
		PaStream *stream;
		PaStreamParameters output_params = {};
		output_params.channelCount = channel;
		output_params.device = device;
		output_params.hostApiSpecificStreamInfo = nullptr;
		output_params.sampleFormat = format;
		output_params.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
		if(paNoError == (ret_er = Pa_OpenStream(&stream, nullptr, &output_params, rate,
				paFramesPerBufferUnspecified, paNoFlag, nullptr, nullptr))) {
			ret = action(stream);
			Pa_CloseStream(stream);
		} else
			ret = log_errno(ret_er);
		Pa_Terminate();
	} else
		ret = log_errno(ret_er);
	return ret;
}

