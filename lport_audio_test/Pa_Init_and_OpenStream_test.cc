#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lport_audio/lport_audio.h"
#include "mock_port_audio/mock_port_audio.h"

SUITE_START("Pa_Init_and_OpenStream_test");

static int arg_rate, arg_channels;
static PaTime ret_latency;
static PaSampleFormat arg_format;
static PaStream *ret_stream;
static PaDeviceIndex arg_device;
static PaDeviceInfo ret_device;

mock_function_1(int, open_stream_action, PaStream *);
static PaError stub_Pa_Init_OpenOutputStream(PaStream **stream, const PaStreamParameters *, const PaStreamParameters *, double, unsigned long, PaStreamFlags, PaStreamCallback *, void *) {
	*stream = ret_stream;
	return paNoError;
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_device = 100;
	arg_rate = 48000;
	arg_channels = 6;
	arg_format = paInt32;
	ret_stream = (PaStream *)&ret_stream;
	ret_device.defaultLowInputLatency = ret_latency = 100;

	init_mock_function_with_return(Pa_Initialize, paNoError)
	init_mock_function_with_function(Pa_OpenStream, stub_Pa_Init_OpenOutputStream);
	init_mock_function_with_return(Pa_StartStream, paNoError);
	init_mock_function_with_return(Pa_GetDeviceInfo, &ret_device);
	init_mock_function_with_return(Pa_StopStream, paNoError);
	init_mock_function_with_return(Pa_CloseStream, paNoError);
	init_mock_function_with_return(Pa_Terminate, paNoError);
	init_mock_function(open_stream_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return Pa_Init_OpenOutputStream(arg_device, arg_rate, arg_channels, arg_format, open_stream_action);
}

static PaError assert_open_stream(PaStream **stream, const PaStreamParameters *, const PaStreamParameters *output_params, double rate, unsigned long, PaStreamFlags, PaStreamCallback *, void *) {
	*stream = ret_stream;
	CUE_ASSERT_EQ((int)rate, arg_rate);
	CUE_ASSERT_EQ(output_params->channelCount, arg_channels);
	CUE_ASSERT_EQ(output_params->device, arg_device);
	CUE_ASSERT_PTR_EQ(output_params->hostApiSpecificStreamInfo, nullptr);
	CUE_ASSERT_EQ(output_params->sampleFormat, arg_format);
	CUE_ASSERT_EQ((int)output_params->suggestedLatency, (int)ret_latency);
	return paNoError;
}

SUITE_CASE("init and open audio") {
	init_mock_function_with_function(Pa_OpenStream, assert_open_stream);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(Pa_Initialize);

	CUE_EXPECT_CALLED_ONCE(Pa_OpenStream);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_OpenStream, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(Pa_OpenStream, 5, paFramesPerBufferUnspecified);
	CUE_EXPECT_CALLED_WITH_INT(Pa_OpenStream, 6, paNoFlag);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_OpenStream, 7, nullptr);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_OpenStream, 8, nullptr);

	CUE_EXPECT_CALLED_ONCE(Pa_StartStream);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_StartStream, 1, ret_stream);

	CUE_EXPECT_CALLED_ONCE(open_stream_action);
	CUE_EXPECT_CALLED_WITH_PTR(open_stream_action, 1, ret_stream);

	CUE_EXPECT_CALLED_ONCE(Pa_StopStream);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_StopStream, 1, ret_stream);

	CUE_EXPECT_CALLED_ONCE(Pa_CloseStream);
	CUE_EXPECT_CALLED_WITH_PTR(Pa_CloseStream, 1, ret_stream);

	CUE_EXPECT_CALLED_ONCE(Pa_Terminate);
}

SUITE_CASE("failed init audio") {
	init_mock_function_with_return(Pa_Initialize, paInternalError);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(Pa_OpenStream);

	CUE_EXPECT_NEVER_CALLED(Pa_StartStream);

	CUE_EXPECT_NEVER_CALLED(open_stream_action);

	CUE_EXPECT_NEVER_CALLED(Pa_StopStream);

	CUE_EXPECT_NEVER_CALLED(Pa_CloseStream);

	CUE_EXPECT_NEVER_CALLED(Pa_Terminate);

	CUE_ASSERT_STDERR_EQ("Error[liblportaudio]: port audio error\n");
}

SUITE_CASE("failed to open stream") {
	init_mock_function_with_return(Pa_OpenStream, paInternalError);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(Pa_StartStream);

	CUE_EXPECT_NEVER_CALLED(open_stream_action);

	CUE_EXPECT_NEVER_CALLED(Pa_StopStream);

	CUE_EXPECT_NEVER_CALLED(Pa_CloseStream);

	CUE_ASSERT_STDERR_EQ("Error[liblportaudio]: port audio error\n");
}

SUITE_CASE("failed to start stream") {
	init_mock_function_with_return(Pa_StartStream, paInternalError);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(open_stream_action);

	CUE_EXPECT_NEVER_CALLED(Pa_StopStream);

	CUE_ASSERT_STDERR_EQ("Error[liblportaudio]: port audio error\n");
}

SUITE_END(Pa_Init_and_OpenStream_test);


