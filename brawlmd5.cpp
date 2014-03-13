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

String^ BrawlLS::MD5::MD5Str(ResourceNode^ node) {
	// Requires BrawlLib 0.18L2 (libertyernie's version) or newer
	return node->MD5Str();
}

/* Take the MD5 sums of the concatenated raw data for each node in the list.
This does not reset indices in the headers to 0, like the function in
ResourceNode does, but in this case it's not necessary - nodes in a different
order will lead to a different MD5 for the parent anyway. */
String^ BrawlLS::MD5::MD5Str(IEnumerable<ResourceNode^>^ nodes) {
	MD5_CTX context;
	MD5_Init(&context);
	for each(ResourceNode^ node in nodes) {
		char* addr = (char*)(void*)node->OriginalSource.Address;
		int len = node->OriginalSource.Length;
		if (len > 0) {
			MD5_Update(&context, addr, len);
		}
	}
	unsigned char result[16];
	MD5_Final(result, &context);

	StringBuilder sb;
	for (int i = 0; i < 16; i++) {
		sb.Append(result[i].ToString("x2"));
	}
	return sb.ToString();
}
