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
	FILE* in;
	if (argc == 1) {
		in = stdin;
	} else {
		in = fopen(argv[1], "rb");
	}

	cout << "Addr: " << VALUES32B_HEADER << endl;
	for (int i = 0; i < strlen(VALUES32B_HEADER) + 6; i++) {
		cout << '-';
	}
	cout << endl;

	char* buf = new char[V32B_BUFFER];
	int bytes_read;
	size_t add_to_addr = 0;
	while ((bytes_read = fread(buf, 1, V32B_BUFFER, in)) > 0) {
		values32b_to_cout(buf, bytes_read, add_to_addr);
		add_to_addr += bytes_read;
	}
	delete[] buf;
}
