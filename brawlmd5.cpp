#include "md5.h"
#include "brawlmd5.h"
#include <cstdlib>

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace BrawlLib::SSBB::ResourceNodes;

String^ BrawlLS::MD5::MD5Str(void* addr, int len) {
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, addr, len);
	unsigned char result[16];
	MD5_Final(result, &context);

	StringBuilder sb;
	for (int i = 0; i < 16; i++) {
		sb.Append(result[i].ToString("x2"));
	}
	return sb.ToString();
}

String^ BrawlLS::MD5::MD5Str(String^ pathname) {
	array<unsigned char>^ arr = File::ReadAllBytes(pathname);
	unsigned char* ptr = (unsigned char*)malloc(arr->Length);
	System::Runtime::InteropServices::Marshal::Copy(arr, 0, (IntPtr)ptr, arr->Length);
	String^ s = MD5Str(ptr, arr->Length);
	free(ptr);
	return s;
}

String^ BrawlLS::MD5::MD5Str(ResourceNode^ node, int skip_bytes) {
	array<ResourceNode^>^ arr = gcnew array<ResourceNode^>(1);
	arr[0] = node;
	return MD5Str(arr, skip_bytes);
}

String^ BrawlLS::MD5::MD5Str(IEnumerable<ResourceNode^>^ nodes, int skip_bytes) {
	MD5_CTX context;
	MD5_Init(&context);
	for each(ResourceNode^ node in nodes) {
		char* addr = (char*)(void*)node->OriginalSource.Address;
		int len = node->OriginalSource.Length;
		if (node->Name == "Tower8") {
			array<unsigned char>^ arr = gcnew array<unsigned char>(len);
			System::Runtime::InteropServices::Marshal::Copy((IntPtr)addr, arr, 0, len);
			System::IO::File::WriteAllBytes("a.bin", arr);
		}
		if (skip_bytes > 0) {
			addr += skip_bytes;
			len -= skip_bytes;
		}
		if (len > 0) {
			MD5_Update(&context, addr, len);
		}
	}
	unsigned char result[16];
	MD5_Final(result, &context);

	StringBuilder sb(skip_bytes > 0 ? "(" + skip_bytes + ")" : "R");
	for (int i = 0; i < 16; i++) {
		sb.Append(result[i].ToString("x2"));
	}
	return sb.ToString();
}
