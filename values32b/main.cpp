#include "../values32b.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using std::cout;
using std::cerr;
using std::endl;

const char* USAGE = R"(Usage: values32b [--no-header] [filename]
If the filename is omitted, the program will read from standard input.
For license info run: values32b --license
)";

const char* LICENSE = R"(values32b
(C) 2014 libertyernie

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
)";

bool hashelparg(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "--help") == 0) {
			return true;
		}
	}
	return false;
}

bool haslicensearg(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--license") == 0) {
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
	bool use_header = true;
	while (argc > 1 && strcmp("--no-header", argv[1]) == 0) {
		use_header = false;
		argv++;
		argc--;
	}
	if (argc > 1) {
		if (hashelparg(argc, argv)) {
			cout << USAGE;
			return EXIT_SUCCESS;
		} else if (haslicensearg(argc, argv)) {
			cout << LICENSE;
			return EXIT_SUCCESS;
		} else if (argc > 2) {
			cerr << USAGE;
			return EXIT_FAILURE;
		}
	}

	VALUES32B_OPTIONS opts;
	opts.add_to_address_printout = 0;
	opts.min_addr_digits = 2;
	opts.prefix = NULL;

	FILE* in;
	if (argc == 1) {
		in = stdin;
	} else {
		in = fopen(argv[1], "rb");

		if (in == NULL) {
			cerr << "Cannot open file: " << argv[1] << endl;
			cerr << USAGE;
			return EXIT_FAILURE;
		}

		fseek(in, 0, SEEK_END);
		opts.increase_min_addr_digits(ftell(in)); // expand digit space to show max address of the file
		fseek(in, 0, SEEK_SET);
	}

	char* buf = new char[V32B_BUFFER];
	int bytes_read = fread(buf, 1, V32B_BUFFER, in); // read first chunk
	opts.increase_min_addr_digits(bytes_read); // expand digit space to show max address of this chunk
	if (use_header) print_header(opts.min_addr_digits);

	while (bytes_read > 0) {
		values32b_to_cout(buf, bytes_read, opts);
		opts.add_to_address_printout += bytes_read;
		bytes_read = fread(buf, 1, V32B_BUFFER, in); // read next chunk
	}
	delete[] buf;
}
