#include <vector>
#include <functional>
#include <algorithm>
#include "iobus/iobus.h"
#include "media/media.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"

int main() {
	iobus iob(stdin, stdout, stderr);
	std::vector<int> receivers;

	std::function<int(int)> control_action = [&](int i) {
		receivers.push_back(i);
		while(control_event(iob, control_action)) {
			while(!iob.get("COMMAND", [&](const char *args) {
					std::for_each(receivers.begin(), receivers.end(), [&](int id) {
							msgsnd(id, args, []{return 0;});
							});
					return 0;
					}))
			;
		}
		return 0;
	};
	return ignore_untill(iob, control_event, control_action);
}

