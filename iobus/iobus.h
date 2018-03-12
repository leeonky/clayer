#ifndef IOBUS_H
#define IOBUS_H
#include <functional>
#include <cstdlib>

class iobus {
public:
	iobus(FILE *in, FILE *out, FILE *err) : file_in(in), file_out(out), file_err(err), line(nullptr), arguments(""), processed(true) {}
	~iobus() { free(line); }

	void post(const char *format, ...);

	void recaption_and_post();

	int get(const std::function<int(const char *, const char *)> &);
	int get(const char *, const std::function<int(const char *, const char *)> &, int, const char *, ...);
	int get(const char *event, const std::function<int(const char *, const char *)> &action) {
		return get(event, action, 0, "");
	}
	int pass_through();
	void accept_processed() {
		processed = true;
	}

	int except(const char *);
private:
	FILE *file_in, *file_out, *file_err;
	char *line;
	char command[128];
	const char *arguments;
	bool processed;
};


#endif
