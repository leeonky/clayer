#ifndef SUB_SRT_H
#define SUB_SRT_H

#include <cstdio>
#include <string>
#include <vector>
#include "iobus/iobus.h"
#include "stdport/stdport.h"

class subtitle_srt_item {
public:
	std::string content;
	int64_t from, to;
	int index;
};

class subtitle_srt {
public:
	subtitle_srt(FILE *);
	void query_item(int64_t, const std::function<void(const std::string &)> &);
	const std::vector<subtitle_srt_item>::const_iterator get_item(int64_t);

private:
	std::vector<subtitle_srt_item> items;
	std::vector<subtitle_srt_item>::const_iterator last_searched, last_shown;
};

#endif
