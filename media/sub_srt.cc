#include "sub_srt.h"
#include <cstring>
#include <algorithm>

namespace {
	int64_t to_use(int h, int m, int s, int ms) {
		int64_t sec = h*3600 + m*60 + s;
		sec *= 1000;
		sec += ms;
		return sec * 1000;
	}

	int get_line(FILE *input, const std::function<bool(const char *)> &action) {
		char *line = NULL;
		size_t linecap = 0;
		ssize_t linelen;
		while ((linelen = getline(&line, &linecap, input)) > 0) {
			if(action(line))
				return 0;
		}
		return -1;
	}
}

subtitle_srt::subtitle_srt(FILE *input) :last_item(0) {
	subtitle_srt_item item;
	while(0==get_line(input, [&](const char *line) { return 1 == sscanf(line, "%d", &item.index); }) 
		&& 0==get_line(input, [&](const char *line) {
			int fh, fm, fs, fms, th, tm, ts, tms;
			bool ret;
			if(ret = (8==sscanf(line, "%d:%d:%d,%d --> %d:%d:%d,%d", &fh, &fm, &fs, &fms, &th, &tm, &ts, &tms))) {
				item.from = to_use(fh, fm, fs, fms);
				item.to = to_use(th, tm, ts, tms);
			}
			return ret;
		})) {
		get_line(input, [&](const char *line) {
			if(strcmp(line, "\n") && strcmp(line, "\r\n")) {
				item.content += line;
				return false;
			} else
				return true;
		});
		items.push_back(item);
	}
}

const std::vector<subtitle_srt_item>::const_iterator subtitle_srt::get_item(int64_t time) {
	if(time >= items[last_item].from) {
		for(; last_item<items.size(); ++last_item) {
			if(time >= items[last_item].from && time <= items[last_item].to)
				return items.begin() + last_item;
			else if(time <= items[last_item].from)
				break;
		}
		return items.end();
	} else {
		last_item = 0;
		return get_item(time);
	}
}

