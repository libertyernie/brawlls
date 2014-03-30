#include <iosfwd>
void values32b_to(std::ostream& out, const char* prefix, void* address, size_t bytelength);
void values32b_to_cout(void* address, size_t bytelength);

#ifdef __cplusplus_cli
System::String^ values32b_to_clistr(System::String^ prefix, BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
#endif
