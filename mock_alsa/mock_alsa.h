#ifndef MOCK_PORT_AUDIO_
#define MOCK_PORT_AUDIO_

#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <alsa/asoundlib.h>

extern_mock_function_4(int, snd_pcm_open, snd_pcm_t **, const char *, snd_pcm_stream_t, int);
extern_mock_function_1(int, snd_pcm_hw_params_malloc, snd_pcm_hw_params_t **);
extern_mock_function_2(int, snd_pcm_hw_params_any, snd_pcm_t *, snd_pcm_hw_params_t *);
extern_mock_function_3(int, snd_pcm_hw_params_set_access, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_access_t);
extern_mock_function_3(int, snd_pcm_hw_params_set_format, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_format_t);
extern_mock_function_4(int, snd_pcm_hw_params_set_rate, snd_pcm_t *, snd_pcm_hw_params_t *, unsigned, int);
extern_mock_function_3(int, snd_pcm_hw_params_set_channels, snd_pcm_t *, snd_pcm_hw_params_t *, unsigned);
extern_mock_function_3(int, snd_pcm_hw_params_set_buffer_size, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t);
extern_mock_function_4(int, snd_pcm_hw_params_set_period_size, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t, int);
extern_mock_function_2(int, snd_pcm_hw_params, snd_pcm_t *, snd_pcm_hw_params_t *);
extern_mock_void_function_1(snd_pcm_hw_params_free, snd_pcm_hw_params_t *);
extern_mock_function_1(int, snd_pcm_close, snd_pcm_t *);
extern_mock_function_3(snd_pcm_sframes_t, snd_pcm_mmap_writei, snd_pcm_t *, const void *, snd_pcm_uframes_t);
extern_mock_function_1(int, snd_pcm_prepare, snd_pcm_t *);
extern_mock_function_2(int, snd_pcm_delay, snd_pcm_t *, snd_pcm_sframes_t *);
extern_mock_function_3(int, snd_pcm_hw_params_set_buffer_size_near, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *);
extern_mock_function_4(int, snd_pcm_hw_params_set_period_size_near, snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *, int *);

#ifdef __cplusplus
}
#endif

#endif
