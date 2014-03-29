#include <string>
#include <cstdio>

using namespace std;

using System::String;
using System::Text::StringBuilder;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

unsigned char safe_c(unsigned char c) {
	return isprint(c) ? c : ' ';
}

union entry_4byte {
	float f;
	__int32 i;

	char* ptr() {
		return (char*)&i;
	}

	static const size_t SUMMARY_SIZE = 43;
	void summary_to_buffer(char* dest) {
		char* self_ptr = ptr();
		char as_ascii[4];
		for (int z = 0; z < 4; z++) {
			as_ascii[z] = safe_c(self_ptr[z]);
		}
		sprintf(dest, "%10d / %12g (%.4s) %08X", i, f, as_ascii, i);
	}
	
	String^ summary_to_cli_str() {
		char buf[SUMMARY_SIZE];
		summary_to_buffer(buf);
		return gcnew String(buf);
	}

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

String^ stdt_lines(String^ prefix, ResourceNode^ node) {
	StringBuilder sb;
	entry_4byte* addr = (entry_4byte*)(void*)node->UncompressedSource.Address;
	int length = node->UncompressedSource.Length;
	for (int i = 5; i < length; i++) {
		sb.AppendLine("0x" + (i*4).ToString("X3") + ": " + addr[i].rv_endian().summary_to_cli_str());
	}
	return sb.ToString();
}
