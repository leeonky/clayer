#ifndef IOBUS_H
#define IOBUS_H
#include <functional>
#include <cstdlib>

class iobus {
public:
	iobus(FILE *in, FILE *out, FILE *err) : file_in(in), file_out(out), file_err(err), line(nullptr), arguments(""), processed(true) {}
	~iobus() { free(line); }

	void post(const char *format, ...);

	int get(const std::function<int(const char *, const char *)> &);
	int get(const char *, const std::function<int(void)> &, int, const char *, ...);
	int get(const char *event, const std::function<int(const char *)> &action);

	int forward_last() {
		if(processed)
			return -1;
		fprintf(file_out, "%s", line);
		fflush(file_out);
		accept_processed();
		return 0;
	}

	int ignore_last() {
		if(processed)
			return -1;
		accept_processed();
		return 0;
	}

	int pass_through();
	int except(const char *);
	void recaption_and_post();

private:
	FILE *file_in, *file_out, *file_err;
	char *line;
	char command[128];
	const char *arguments;
	bool processed;

	void accept_processed() {
		processed = true;
	}
};


#endif
