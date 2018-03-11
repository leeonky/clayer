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

extern_mock_function_0(DrawingWand *, NewDrawingWand);
extern_mock_void_function_2(DrawSetGravity, DrawingWand *, GravityType);
extern_mock_void_function_2(DrawSetTextEncoding, DrawingWand *, const char *);
extern_mock_function_1(DrawingWand *, DestroyDrawingWand, DrawingWand *);

extern_mock_function_2(MagickBooleanType, DrawSetFont, DrawingWand *, const char *);
extern_mock_void_function_2(DrawSetFontSize, DrawingWand *, double);

extern_mock_function_0(PixelWand *, NewPixelWand);
extern_mock_function_2(MagickBooleanType, PixelSetColor, PixelWand *, const char *);
extern_mock_void_function_2(DrawSetFillColor, DrawingWand *, const PixelWand *);
extern_mock_function_1(PixelWand *, DestroyPixelWand, PixelWand *);

#ifdef __cplusplus
}
#endif

#endif

