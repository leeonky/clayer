#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"
#include "lmagic_wand/lmagic_wand.h"

int main(int argc, char **argv) {
	int w=-1, h=-1;
	command_argument().require_full_argument("size", 's', [&](const char *arg){
			sscanf(arg, "%dx%d", &w, &h);
			}).parse(argc, argv);
	if(!argv[1]) {
		fprintf(app_stderr, "Error[subtitle]: require subtitle file\n");
		return -1;
	}

	return fopen(argv[1], "rb", [&](FILE *sub_file){
		iobus iob(stdin, stdout, stderr);

		return video_event(iob, [&](int fw, int fh, enum AVPixelFormat av_format){
				w = w==-1 ? fw : w;
				h = h==-1 ? fh : h;
				iob.recaption_and_post();
				return buffer_event(iob, [&](int shmid, size_t size, int count, int semid, int video_buffer_key) {
						iob.recaption_and_post();
						return circular_shm::create(w*h*4, count/2,
								[&](circular_shm &out_shm){
								iob.post("%s", out_shm.serialize_to_string());

								//while((!frames_event(iob, [&](frame_list &frames){
										//for(int i=0; i<frames.count; i++){
											//frames.frames[i].timestamp
										//}
										//return 0;
										//})) || (!clock_event(iob, [&](int64_t base, int64_t offset){
											//clock.sync(base, offset);
											//return 0;
											//})))
								//;
								return 0;
								});
						});
				});

		});
}

