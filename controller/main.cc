#include <vector>
#include <functional>
#include <algorithm>
#include "iobus/iobus.h"
#include "media/media.h"
#include "stdexd/stdexd.h"
#include "sysexd/sysexd.h"

int main() {
	iobus iob(stdin, stdout, stderr);

	while(!ignore_untill(iob, control_event, [&](int receiver) {
				while(!iob.get("COMMAND", [&](const char *args) {
							msgsnd(receiver, args, []{return 0;});
							return 0;
							}))
				;
				return 0;
				}))
	;
	return 0;
}

