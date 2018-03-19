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

subtitle_srt::subtitle_srt(FILE *input) :time_hited(false) {
	subtitle_srt_item item;
	while(0==get_line(input, [&](const char *line) { return 1 == sscanf(line, "%d", &item.index); }) 
		&& 0==get_line(input, [&](const char *line) {
			int fh, fm, fs, fms, th, tm, ts, tms;
			bool ret;
			if((ret = (8==sscanf(line, "%d:%d:%d,%d --> %d:%d:%d,%d", &fh, &fm, &fs, &fms, &th, &tm, &ts, &tms)))) {
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
		item.content = "";
	}
	std::sort(items.begin(), items.end(), [](const subtitle_srt_item &item1, const subtitle_srt_item &item2) {
			return item1.from < item2.from;
			});
	last_searched = items.begin();
	last_shown = items.end();
}

void subtitle_srt::query_item(int64_t time, const std::function<void(const std::string &)> &action) {
	if(items.size()) {
		if(time >= last_searched->from || last_searched == items.begin()) {
			for(; last_searched != items.end(); ++last_searched) {
				if(time >= last_searched->from && time <= last_searched->to) {
					if(last_searched != last_shown) {
						last_shown = last_searched;
						time_hited = true;
						action(last_searched->content);
					}
					return;
				}
				else if(time <= last_searched->from)
					break;
			}
			if(time_hited) {
				time_hited = false;
				action("");
			}
			return;
		} else {
			last_searched = items.begin();
			return query_item(time, action);
		}
	}
}

