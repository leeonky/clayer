#ifndef MOCK_PORT_AUDIO_
#define MOCK_PORT_AUDIO_

#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <ass/ass.h>

extern_mock_function_0(ASS_Library *, ass_library_init);
extern_mock_void_function_1(ass_library_done, ASS_Library *);
extern_mock_function_1(ASS_Renderer *, ass_renderer_init, ASS_Library *);
extern_mock_void_function_1(ass_renderer_done, ASS_Renderer *);
extern_mock_void_function_3(ass_set_frame_size, ASS_Renderer *, int, int);
extern_mock_void_function_6(ass_set_fonts, ASS_Renderer *, const char *, const char *, int, const char *, int);
extern_mock_function_3(ASS_Track *, ass_read_file, ASS_Library *, char *, char *);
extern_mock_void_function_1(ass_free_track, ASS_Track *);
extern_mock_function_4(ASS_Image *, ass_render_frame, ASS_Renderer *, ASS_Track *, long long, int *);

#ifdef __cplusplus
}
#endif

#endif
