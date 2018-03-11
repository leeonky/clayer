#include "lmagic_wand.h"
#include "stdexd/stdexd.h"

namespace {
	int log_error(const MagickWand *wand) {
		ExceptionType type;
		const char *message = MagickGetException(wand, &type);
		return ::log_error("liblmagic_wand", "%s", message);
	}

	int log_error(const DrawingWand *wand) {
		ExceptionType type;
		const char *message = DrawGetException(wand, &type);
		return ::log_error("liblmagic_wand", "%s", message);
	}
}

int NewMagickWand(size_t w, size_t h, const char *file, const std::function<int(MagickWand *)> &action) {
	int ret = 0;
	if(MagickWand *wand = NewMagickWand()) {
		if(MagickSetSize(wand, w, h) == MagickTrue
				&& MagickReadImage(wand, file) == MagickTrue)
			ret = action(wand);
		else
			ret = log_error(wand);
		DestroyMagickWand(wand);
	} else
		ret = log_error(wand);
	return ret;
}

int NewDrawingWand(const std::function<int(DrawingWand *)> &action) {
	int ret = 0;
	if(DrawingWand *wand = NewDrawingWand()) {
		DrawSetGravity(wand, NorthWestGravity);
		DrawSetTextEncoding(wand, "UTF8");
		ret = action(wand);
		DestroyDrawingWand(wand);
	} else
		ret = log_error(wand);
	return ret;
}

int DrawSetFont(DrawingWand *wand, int size, const char *font) {
	int ret = 0;
	if(DrawSetFont(wand, font) == MagickTrue)
		DrawSetFontSize(wand, size);
	else
		ret = log_error(wand);
	return ret;
}

void DrawSetFillColor(DrawingWand *wand, const char *color) {
	PixelWand *pwand = NewPixelWand();
	PixelSetColor(pwand, color);
	DrawSetFillColor(wand, pwand);
	DestroyPixelWand(pwand);
}

