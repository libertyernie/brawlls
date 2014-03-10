#include "md5.h"
#include "brawlmd5.h"

using namespace System;
using namespace System::Text;
using namespace BrawlLib::SSBB::ResourceNodes;

String^ md5str(void* addr, int len) {
	return BrawlLS::Methods::MD5Str(addr, len);
}

String^ md5str(ResourceNode^ node) {
	return BrawlLS::Methods::MD5Str(node);
}

String^ BrawlLS::Methods::MD5Str(void* addr, int len) {
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

String^ BrawlLS::Methods::MD5Str(ResourceNode^ node) {
	void* addr = node->OriginalSource.Address;
	int len = node->OriginalSource.Length;
	return MD5Str(addr, len);
}
