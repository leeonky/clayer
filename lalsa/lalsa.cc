#include "lalsa.h"
#include "stdexd/stdexd.h"

#undef log_error
#define log_error(ret) log_error("liblalsa", "%s", snd_strerror(ret))

#undef log_warning
#define log_warning(format, ...) log_warning("liblalsa", (format), ## __VA_ARGS__)

int snd_pcm_open(const char *dev, int rate, int channel, snd_pcm_format_t format, const std::function<int(snd_pcm_t *)> &action) {
	int res = 0;
	snd_pcm_t *pcm_t = NULL;
	snd_pcm_hw_params_t *hw_params = NULL;
	if(!(res = snd_pcm_open(&pcm_t, dev, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC))) {
		if(!(res = snd_pcm_hw_params_malloc(&hw_params))) {
			if(!(res = snd_pcm_hw_params_any(pcm_t, hw_params))
					&& !(res = snd_pcm_hw_params_set_access(pcm_t, hw_params, SND_PCM_ACCESS_MMAP_INTERLEAVED))
					&& !(res = snd_pcm_hw_params_set_rate(pcm_t, hw_params, rate, 0))
					&& !(res = snd_pcm_hw_params_set_channels(pcm_t, hw_params, channel))
					&& !(res = snd_pcm_hw_params_set_format(pcm_t, hw_params, format))
					&& !(res = snd_pcm_hw_params_set_buffer_size(pcm_t, hw_params, rate/10))
					&& !(res = snd_pcm_hw_params_set_period_size(pcm_t, hw_params, rate/50, 0))
					&& !(res = snd_pcm_hw_params(pcm_t, hw_params))){
				res = action(pcm_t);
			} else
				res = log_error(res);
			snd_pcm_hw_params_free(hw_params);
		} else
			res = log_error(res);
		snd_pcm_close(pcm_t);
	} else
		res = log_error(res);
	return res;
}

int snd_pcm_mmap_writei(snd_pcm_t *pcm, const void *buffer, int frames) {
	snd_pcm_sframes_t ret;
	while((ret = snd_pcm_mmap_writei(pcm, buffer, (snd_pcm_uframes_t)frames))<0) {
		if(ret == -EPIPE) {
			int res;
			log_warning("xrun");
			if((res = snd_pcm_prepare(pcm)))
				return log_error(res);
		} else
			return log_error(ret);
	}
	return 0;
}

int64_t snd_pcm_delay(snd_pcm_t *pcm, int rate) {
	snd_pcm_sframes_t t;
	int res = snd_pcm_delay(pcm, &t);
	if(res) {
		log_error(res);
		return 0;
	}
	return ((int64_t)t)*1000000/rate;
}

