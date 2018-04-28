#ifndef SUB_ASS_H
#define SUB_ASS_H

#include <ass/ass.h>
#include <functional>

extern int subtitle_ass(const char *, int, int, const char *, const std::function<void(ASS_Renderer *, ASS_Track *)> &);
extern void ass_render_frame(ASS_Renderer *, ASS_Track *, int64_t, const std::function<void(ASS_Image *)> &);

#endif

