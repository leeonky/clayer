#include "mock_alsa.h"

mock_function_4(int, snd_pcm_open, snd_pcm_t **, const char *, snd_pcm_stream_t, int);
mock_function_1(int, snd_pcm_hw_params_malloc, snd_pcm_hw_params_t **);
mock_function_2(int, snd_pcm_hw_params_any, snd_pcm_t *, snd_pcm_hw_params_t *);
mock_function_3(int, snd_pcm_hw_params_set_access, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_access_t);
mock_function_3(int, snd_pcm_hw_params_set_format, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_format_t);
mock_function_4(int, snd_pcm_hw_params_set_rate, snd_pcm_t *, snd_pcm_hw_params_t *, unsigned, int);
mock_function_3(int, snd_pcm_hw_params_set_channels, snd_pcm_t *, snd_pcm_hw_params_t *, unsigned);
mock_function_3(int, snd_pcm_hw_params_set_buffer_size, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t);
mock_function_4(int, snd_pcm_hw_params_set_period_size, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t, int);
mock_function_2(int, snd_pcm_hw_params, snd_pcm_t *, snd_pcm_hw_params_t *);
mock_void_function_1(snd_pcm_hw_params_free, snd_pcm_hw_params_t *);
mock_function_1(int, snd_pcm_close, snd_pcm_t *);
mock_function_3(snd_pcm_sframes_t, snd_pcm_mmap_writei, snd_pcm_t *, const void *, snd_pcm_uframes_t);
mock_function_1(int, snd_pcm_prepare, snd_pcm_t *);
mock_function_2(int, snd_pcm_delay, snd_pcm_t *, snd_pcm_sframes_t *);
mock_function_3(int, snd_pcm_hw_params_set_buffer_size_near, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *);
mock_function_4(int, snd_pcm_hw_params_set_period_size_near, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *, int *);

const char *snd_strerror(int ret) {
	static char buffer[128];
	sprintf(buffer, "%d", ret);
	return buffer;
}
