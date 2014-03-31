#include <iosfwd>
void values32b_to(std::ostream& out, const char* prefix, void* address, size_t bytelength, size_t add_to_addr);
void values32b_to(std::ostream& out, const char* prefix, void* address, size_t bytelength);
void values32b_to_cout(void* address, size_t bytelength, size_t add_to_addr);
void values32b_to_cout(void* address, size_t bytelength);

const char* const VALUES32B_HEADER = "UnsignedInt / FloatingPoint (Text) HexBytes";

#ifdef __cplusplus_cli
void values32b_to_cout(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
System::String^ values32b_to_clistr(System::String^ prefix, BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
#endif
