#include "stdexd/stdexd.h"
#include "mem/circular_shm.h"
#include "iobus/iobus.h"
#include "media/media.h"

int main(int argc, char **argv) {
	int w=-1, h=-1;
	int new_key = 2;
	int new_count = 16;
	circular_shm *shms[MAX_LAYER_COUNT];
	const char *file_name = command_argument().require_full_argument("size", 's', [&](const char *arg){
			sscanf(arg, "%dx%d", &w, &h);
			}).parse(argc, argv);
	if(!file_name) {
		fprintf(app_stderr, "Error[subtitle]: require subtitle file\n");
		return -1;
	}

	return fopen(file_name, "rb", [&](FILE *sub_file){
		iobus iob(stdin, stdout, stderr);

		return ignore_untill(iob, video_event, [&](int fw, int fh, enum AVPixelFormat av_format){
				w = w==-1 ? fw : w;
				h = h==-1 ? fh : h;
				iob.recaption_and_post();

				return circular_shm::create(w*h*4, new_count,
						[&](circular_shm &shm){
						iob.post("%s", shm.serialize_to_string(new_key));

						return main_transform(iob, shms, frame_event, [&](int, int, int64_t pts) {
								iob.recaption_and_post();
								//iob.post("LAYER buffer:%d 0=>0,0,200,100", new_key);
								return 0;
								});
						});
				});

		});
}

