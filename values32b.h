#include <iosfwd>
#ifdef __cplusplus_cli
void values32b_to(System::IO::TextWriter^ out, System::String^ prefix,
	BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
#endif
void values32b_to(std::ostream& out, const char* prefix, void* address, size_t bytelength);
void values32b_to_cout(void* address, size_t bytelength);
