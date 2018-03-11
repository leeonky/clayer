#include "lmagic_wand.h"
#include "stdexd/stdexd.h"

namespace {
	int log_error(const MagickWand *wand) {
		ExceptionType type;
		const char *message = MagickGetException(wand, &type);
		return ::log_error("liblmagic_wand", "%s", message);
	}
}

int NewMagickWand(size_t w, size_t h, const char *file, const std::function<int(MagickWand *)> &action) {
	int ret = 0;
	if(MagickWand *wand = NewMagickWand()) {
		if(MagickSetSize(wand, w, h) == MagickTrue) {
			MagickReadImage(wand, file);
			ret = action(wand);
		} else
			ret = log_error(wand);
		DestroyMagickWand(wand);
	} else
		ret = log_error(wand);
	return ret;
}
