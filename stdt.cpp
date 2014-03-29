#include <string>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

using System::String;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

union entry_4byte {
	float f;
	__int32 i;
	char c;
};

unsigned char safe_c(unsigned char c) {
	return isprint(c) ? c : ' ';
}

ostream& operator<<(ostream& out, entry_4byte u) {
	char hexbuf[9];
	for (int i = 0; i < 4; i++) {
		sprintf(hexbuf, "%08X", u.i);
	}
	out << hexbuf << " (";
	for (int i = 0; i < 4; i++) {
		out << safe_c(*(&u.c + i));
	}
	return out << ") " << setw(10) << u.i << " / " << u.f;
}

entry_4byte rv_endian(entry_4byte input) {
	entry_4byte output;
	char* src = &input.c;
	char* dest = &output.c;
	for (int i = 0; i < 4; i++) dest[i] = src[3 - i];
	return output;
}

bool data_tag_is(const char* tag, ResourceNode^ node) {
	if (node->UncompressedSource.Length < 4) return false;
	char* ptr = (char*)(void*)node->UncompressedSource.Address;
	for (int i = 0; i < 4; i++) {
		if (ptr[i] != tag[i]) return false;
	}
	return true;
}

ostream& operator<<(ostream& out, String^ s) {
	for (int i = 0; i < s->Length; i++) {
		out << (char)s[i];
	}
	return out;
}

String^ stdt_lines(String^ prefix, ResourceNode^ node) {
	stringstream sb;
	entry_4byte* addr = (entry_4byte*)(void*)node->UncompressedSource.Address;
	int length = node->UncompressedSource.Length;
	char addr_str_tmp[4];
	for (int i = 5; i < length; i++) {
		entry_4byte rv = rv_endian(addr[i]);
		sprintf(addr_str_tmp, "%03X", i);
		sb << prefix << "0x" << addr_str_tmp << ": " << rv << endl;
	}
	return gcnew String(sb.str().c_str());
}
