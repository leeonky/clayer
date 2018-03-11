#ifndef MAGIC_WAND_H
#define MAGIC_WAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cunitexd.h>
#include <wand/magick_wand.h>

extern_mock_function_0(MagickWand *, NewMagickWand);
extern_mock_function_2(MagickBooleanType, MagickReadImage, MagickWand *,const char *);
extern_mock_function_1(MagickWand *, DestroyMagickWand, MagickWand *);
extern_mock_function_3(MagickBooleanType, MagickSetSize, MagickWand *, size_t, size_t);

#ifdef __cplusplus
}
#endif

#endif

