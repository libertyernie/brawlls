#include <string>
#include <cstdio>

#ifdef __cplusplus_cli
using System::String;
using System::Text::StringBuilder;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;
#endif

unsigned char safe_c(unsigned char c) {
	return isprint(c) ? c : ' ';
}

union entry_4byte {
	float f;
	__int32 i;

	/* Returns a char* pointer to the union's data. */
	char* ptr() {
		return (char*)&i;
	}

	/* The size of the summary string written by summary_to_buffer. */
	static const size_t SUMMARY_SIZE = 42;

	/* Writes a summary string to the memory pointed at by dest, which must have at least SUMMARY_SIZE bytes available.
	The format of the string is:
	DDDDDDDDDD / FFFFFFFFFFFF (AAAA) HHHHHHHH
	DD = integer representation of data
	FF = floating-point representation of data
	AAAA = ASCII representation of data (non-printable characters replaced with spaces)
	HH = data as four hexadecimal bytes */
	void summary_to_buffer(char* dest) {
		char* self_ptr = ptr();
		char as_ascii[4];
		for (int z = 0; z < 4; z++) {
			as_ascii[z] = safe_c(self_ptr[z]);
		}
		sprintf(dest, "%10d / %12g (%.4s) %08X", i, f, as_ascii, i);
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

bool data_tag_is(const char* tag, ResourceNode^ node) {
	if (node->UncompressedSource.Length < 4) return false;
	char* ptr = (char*)(void*)node->UncompressedSource.Address;
	for (int i = 0; i < 4; i++) {
		if (ptr[i] != tag[i]) return false;
	}
	return true;
}

#ifdef __cplusplus_cli
String^ stdt_lines(String^ prefix, ResourceNode^ node) {
	StringBuilder sb;
	entry_4byte* addr = (entry_4byte*)(void*)node->UncompressedSource.Address;
	int length = node->UncompressedSource.Length / sizeof(entry_4byte);
	for (int i = 5; i < length; i++) {
		char buf[entry_4byte::SUMMARY_SIZE];
		addr[i].rv_endian().summary_to_buffer(buf);
		sb.AppendLine(prefix + "0x" + (i*4).ToString("X3") + ": " + gcnew String(buf));
	}
	return sb.ToString();
}
#endif
