#ifndef LMAGIC_WAND_H
#define LMAGIC_WAND_H

#include <cunitexd.h>
#include <functional>
#include <wand/magick_wand.h>

extern int NewMagickWand(size_t, size_t, const char *, const std::function<int(MagickWand *)> &);

#endif


