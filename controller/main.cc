#include <vector>
#include <functional>
#include <algorithm>
#include "iobus/iobus.h"
#include "media/media.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"

int main() {
	iobus iob(stdin, stdout, stderr);

	std::function<int(int)> control_action = [&](int receiver) {
		while(control_event(iob, control_action)) {
			while(!iob.get("COMMAND", [&](const char *args) {
				msgsnd(receiver, args, []{return 0;});
				return 0;
				}))
			;
		}
		return 0;
	};
	return ignore_untill(iob, control_event, control_action);
}

