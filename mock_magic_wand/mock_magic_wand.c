#include "mock_magic_wand.h"

mock_function_0(MagickWand *, NewMagickWand);
mock_function_2(MagickBooleanType, MagickReadImage, MagickWand *, const char *);
mock_function_1(MagickWand *, DestroyMagickWand, MagickWand *);

mock_function_3(MagickBooleanType, MagickSetSize, MagickWand *, size_t, size_t);

mock_function_0(DrawingWand *, NewDrawingWand);
mock_void_function_2(DrawSetGravity, DrawingWand *, GravityType);
mock_void_function_2(DrawSetTextEncoding, DrawingWand *, const char *);
mock_function_1(DrawingWand *, DestroyDrawingWand, DrawingWand *);

mock_function_2(MagickBooleanType, DrawSetFont, DrawingWand *, const char *);
mock_void_function_2(DrawSetFontSize, DrawingWand *, double);

mock_function_0(PixelWand *, NewPixelWand);
mock_function_2(MagickBooleanType, PixelSetColor, PixelWand *, const char *);
mock_void_function_2(DrawSetFillColor, DrawingWand *, const PixelWand *);
mock_function_1(PixelWand *, DestroyPixelWand, PixelWand *);

char *MagickGetException(const MagickWand *wand, ExceptionType *severity) {
	return "MagickWand error";
}

char *DrawGetException(const DrawingWand *wand, ExceptionType *severity) {
	return "DrawingWand error";
}

