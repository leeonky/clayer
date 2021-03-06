#include <cunitexd.h>

int main() {
	init_test();
	ADD_SUITE(ffmpeg_to_sdl_format);
	ADD_SUITE(video_event_test);
	ADD_SUITE(buffer_event_test);
	ADD_SUITE(frames_event_test);
	ADD_SUITE(media_clock_test);
	ADD_SUITE(audio_event_test);
	ADD_SUITE(samples_event_test);
	ADD_SUITE(resampler_params_test);
	ADD_SUITE(clock_event_test);
	ADD_SUITE(sub_srt_test);
	ADD_SUITE(frame_event_test);
	ADD_SUITE(layer_event_test);
	ADD_SUITE(sample_event_test);
	ADD_SUITE(no_layer_event_test);
	ADD_SUITE(control_event_test);
	ADD_SUITE(create_player_context_test);
	ADD_SUITE(rescaler_params_test);
	ADD_SUITE(sub_ass_test);
	return run_test();
}

