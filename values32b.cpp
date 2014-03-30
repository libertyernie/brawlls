#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef __cplusplus_cli
using System::String;
using System::IO::TextWriter;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;
#endif

const char* HEADER = "UnsignedInt / FloatingPoint (Text) HexBytes";

unsigned char safe_c(unsigned char c) {
	return isprint(c) ? c : ' ';
}

bool is_big_endian() {
	union {
		unsigned __int32 i;
		char c[4];
	} t = { 0x01020304 };
	return t.c[0] == 1;
}

union entry_4byte {
	float f;
	__int32 i;

	/* Returns a char* pointer to the union's data. */
	char* ptr() {
		return (char*)&i;
	}

	/* The size of the summary string written by summary_to_buffer, including null-terminator byte. */
	static const size_t SUMMARY_SIZE = 44;

	/* Writes a summary string to the memory pointed at by dest, which must have at least SUMMARY_SIZE bytes available.
	The format of the string is:
	DDDDDDDDDD / FFFFFFFFFFFF (AAAA) HHHHHHHH
	DD = integer representation of data (big endian)
	FF = floating-point representation of data (big endian)
	AAAA = ASCII representation of data
	HH = data as four hexadecimal bytes */
	void summary_to_buffer(char* dest) {
		char* self_ptr = ptr();
		char as_ascii[4];
		for (int z = 0; z < 4; z++) {
			as_ascii[z] = safe_c(self_ptr[z]);
		}
		entry_4byte x = is_big_endian() ? *this : rv_endian();
		sprintf(dest, "% 11d / % 13g (%.4s) %08X", x.i, x.f, as_ascii, x.i);
	}

	/* Creates a new entry_4byte union with the order of the bytes reversed. */
	entry_4byte rv_endian() {
		entry_4byte output;
		char* src = ptr();
		char* dest = output.ptr();
		for (int i = 0; i < 4; i++) dest[i] = src[3 - i];
		return output;
	}
};

/* Write the data printout (in 4-byte fragments) to the specified C++ ostream.
out - the ostream to print to
prefix - a string to append before each line
address, length - the data to print
*/
void values32b_to(std::ostream& out, const char* prefix, void* address, size_t bytelength) {
	entry_4byte* addr = (entry_4byte*)address;
	size_t length = bytelength / sizeof(entry_4byte);

	int min_addr_digits = 0;
	for (size_t l = bytelength; l > 0; l /= 16) {
		min_addr_digits++;
	}

	for (size_t i = 0; i < length; i++) {
		char buf[entry_4byte::SUMMARY_SIZE];
		addr[i].summary_to_buffer(buf);
		out << prefix << "0x" << std::setfill('0') << std::setw(min_addr_digits) << std::hex << i*sizeof(entry_4byte) << ": " << buf << std::endl;
	}
}

void values32b_to_cout(void* address, size_t bytelength) {
	values32b_to(std::cout, "", address, bytelength);
}

#ifdef __cplusplus_cli
/* Return the result of values32b_to(...) as a .NET String. */
String^ values32b_to_clistr(String^ prefix, ResourceNode^ node) {
	char* prefix_buf = new char[prefix->Length + 1];
	for (int i = 0; i < prefix->Length; i++) {
		prefix_buf[i] = (char)prefix[i];
	}
	prefix_buf[prefix->Length] = '\0';

	std::ostringstream oss;
	values32b_to(oss, prefix_buf,
		node->UncompressedSource.Address,
		node->UncompressedSource.Length);
	delete[] prefix_buf;
	
	return gcnew String(oss.str().c_str());
}
#endif
