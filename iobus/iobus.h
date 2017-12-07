#ifndef IOBUS_H
#define IOBUS_H
#include <functional>
#include <cstdlib>

class iobus {
public:
	iobus(FILE *in, FILE *out, FILE *err) : file_in(in), file_out(out), file_err(err), line(nullptr), arguments(""), processed(true) {}
	~iobus() { free(line); }

	void post(const char *message) {
		fprintf(file_out, "%s\n", message);
		fflush(file_out);
	}

	int get(const std::function<int(const char *, const char *)> &);
	int get(const char *, const std::function<int(const char *, const char *)> &, int, const char *, ...);
	int get(const char *event, const std::function<int(const char *, const char *)> &action) {
		return get(event, action, 0, "");
	}
private:
	FILE *file_in, *file_out, *file_err;
	char *line;
	char command[128];
	const char *arguments;
	bool processed;
};


#endif
