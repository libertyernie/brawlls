#include "../values32b.h"
#include <iostream>
#include <cstring>

using namespace std;

bool hashelparg(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "--help") == 0) {
			return true;
		}
	}
	return false;
}

const int V32B_BUFFER = 1048576;

int main(int argc, char** argv) {
	if (argc > 2 || hashelparg(argc, argv)) {
		cerr << "Usage: values32b [filename]" << endl;
		cerr << "If the filename is omitted, the program will read from standard input." << endl;
		return hashelparg(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	VALUES32B_OPTIONS opts;
	opts.add_to_address_printout = 0;
	opts.min_addr_digits = 0;

	FILE* in;
	if (argc == 1) {
		in = stdin;
	} else {
		in = fopen(argv[1], "rb");

		fseek(in, 0, SEEK_END);
		for (long l = ftell(in); l > 0; l /= 16) {
			opts.min_addr_digits++;
		}
		fseek(in, 0, SEEK_SET);
	}

	cout << "Addr: " << VALUES32B_HEADER << endl;
	for (int i = 0; i < strlen(VALUES32B_HEADER) + 6; i++) {
		cout << '-';
	}
	cout << endl;

	char* buf = new char[V32B_BUFFER];
	int bytes_read;
	while ((bytes_read = fread(buf, 1, V32B_BUFFER, in)) > 0) {
		values32b_to_cout(buf, bytes_read, opts);
		opts.add_to_address_printout += bytes_read;
	}
	delete[] buf;
}
