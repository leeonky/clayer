#include "mock_magic_wand.h"

mock_function_0(MagickWand *, NewMagickWand);
mock_function_2(MagickBooleanType, MagickReadImage, MagickWand *, const char *);
mock_function_1(MagickWand *, DestroyMagickWand, MagickWand *);

mock_function_3(MagickBooleanType, MagickSetSize, MagickWand *, size_t, size_t);

char *MagickGetException(const MagickWand *wand,ExceptionType *severity) {
	return "MagickWand error";
}
