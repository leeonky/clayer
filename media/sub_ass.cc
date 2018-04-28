#include <cstring>
#include "stdexd/stdexd.h"
#include "sub_ass.h"

#undef log_error
#define log_error(format, ...) log_error("libmedia", (format), ## __VA_ARGS__)

int subtitle_ass(const char *ass_file, int w, int h, const char *font_file, const std::function<void(ASS_Renderer *, ASS_Track *)> &action) {
	int res = 0;
	char coding_buffer[128], sub_file_buffer[1024];
	strcpy(coding_buffer, "utf-8");
	strcpy(sub_file_buffer, ass_file);

	if(ASS_Library *ass_lib = ass_library_init()) {
		if(ASS_Renderer *ass_renderer = ass_renderer_init(ass_lib)) {
			ass_set_frame_size(ass_renderer, w, h);
			ass_set_fonts(ass_renderer, font_file, NULL, 0, NULL, 0);
			if(ASS_Track *ass_track = ass_read_file(ass_lib, sub_file_buffer, coding_buffer)) {
				action(ass_renderer, ass_track);
				ass_free_track(ass_track);
			} else
				res = log_error("ass_read_file failed");
			ass_renderer_done(ass_renderer);
		} else
			res = log_error("ass_renderer_init failed");
		ass_library_done(ass_lib);
	} else
		res = log_error("ass_library_init failed");
	return res;
}

void ass_render_frame(ASS_Renderer *renderer, ASS_Track *track, int64_t pts, const std::function<void(ASS_Image *)> &action) {
	int new_frame = 0;
	ASS_Image *ass_image = ass_render_frame(renderer, track, pts/1000, &new_frame);
	if(new_frame)
		action(ass_image);
}

