#include "stdexd/stdexd.h"
#include "lffmpeg/lffmpeg.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "lsdl2/lsdl2.h"
#include "media/media.h"

int main(int argc, char **argv) {
	iobus iob(stdin, stdout, stderr);
	const char *arg_rate="", *arg_layout="", *arg_format="";
	command_argument().require_full_argument("rate", 'r', [&](const char *arg){
			arg_rate = arg;
			})
	.require_full_argument("layout", 'l', [&](const char *arg){
			arg_layout = arg;
			})
	.require_full_argument("format", 'f', [&](const char *arg){
			arg_format = arg;
			}).parse(argc, argv);

	return ignore_untill(iob, audio_event, [&](int sample_rate, int /*channels*/, int64_t layout, enum AVSampleFormat format){
			int out_rate = analyze_sample_rate(sample_rate, arg_rate);
			int64_t out_layout  = analyze_channel_layout(layout, arg_layout);
			enum AVSampleFormat out_format = analyze_sample_format(format, arg_format);
			if(out_rate==sample_rate && out_layout==layout && out_format==format) {
				iob.recaption_and_post();
				while(!iob.pass_through());
				return -1;
			}
			return swr_alloc_set_opts_and_init(layout, format, sample_rate, out_layout, out_format, out_rate, [&](resample_context &rs_context) {
					char layout_string_buffer[1024];
					av_get_channel_layout_string(layout_string_buffer, sizeof(layout_string_buffer), rs_context.out_channels, rs_context.out_layout);
					iob.post("AUDIO sample_rate:%d channels:%d layout:%s format:%s", out_rate, rs_context.out_channels, layout_string_buffer, av_get_sample_fmt_name(out_format));
					return buffer_event(iob, [&](int shmid, size_t size, int count, int semid, int audio_buffer_key) {
							return circular_shm::load(shmid, size, count, semid,
									[&](circular_shm &shm){
									return circular_shm::create(rs_context.resample_size(size), count,
											[&](circular_shm &out_shm){
											iob.post("%s", out_shm.serialize_to_string());
											while (!sample_event(iob, [&](int buffer_key, int index, int64_t pts, int samples) {
														shm.free(index, [&](void *buffer){
																int ret = swr_convert(rs_context, buffer, samples, out_shm.allocate());
																if(ret>=0) {
																	iob.post("SAMPLE buffer:%d %d=>%" PRId64 ",%d", audio_buffer_key, out_shm.index, pts, ret);
																}
																});
														return 0;
														}));

											return 0;
											});
									});
					});
			});
	});
}

