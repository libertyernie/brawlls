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

void print_header(size_t addr_digits) {
	for (int i = 2; i < addr_digits; i++) cout << ' ';

	cout << "Addr: " << VALUES32B_HEADER << endl;
	for (int i = 0; i < strlen(VALUES32B_HEADER) + addr_digits + 4; i++) {
		cout << '-';
	}
	cout << endl;
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
	opts.min_addr_digits = 2;
	opts.prefix = nullptr;

	FILE* in;
	if (argc == 1) {
		in = stdin;
	} else {
		in = fopen(argv[1], "rb");

		if (in == NULL) {
			cerr << "Cannot open file: " << argv[1] << endl;
			return EXIT_FAILURE;
		}

		fseek(in, 0, SEEK_END);
		opts.increase_min_addr_digits(ftell(in)); // expand digit space to show max address of the file
		fseek(in, 0, SEEK_SET);
	}

	char* buf = new char[V32B_BUFFER];
	int bytes_read = fread(buf, 1, V32B_BUFFER, in); // read first chunk
	opts.increase_min_addr_digits(bytes_read); // expand digit space to show max address of this chunk
	print_header(opts.min_addr_digits);

	while (bytes_read > 0) {
		values32b_to_cout(buf, bytes_read, opts);
		opts.add_to_address_printout += bytes_read;
		cout << "b,," << bytes_read << endl;
		bytes_read = fread(buf, 1, V32B_BUFFER, in); // read next chunk
	}
	delete[] buf;
}
