#include <iosfwd>

struct VALUES32B_OPTIONS {
	size_t add_to_address_printout;
	size_t min_addr_digits;
	const char* prefix;
};

const VALUES32B_OPTIONS VALUES32B_DEFAULT_OPTIONS{0, 0, nullptr};
void increase_min_addr_digits(VALUES32B_OPTIONS* opts, size_t max_address);

void values32b_to(std::ostream& out, void* address, size_t bytelength, VALUES32B_OPTIONS opts);
void values32b_to(std::ostream& out, void* address, size_t bytelength);
void values32b_to_cout(void* address, size_t bytelength, VALUES32B_OPTIONS opts);
void values32b_to_cout(void* address, size_t bytelength);

const char* const VALUES32B_HEADER = "UnsignedInt / FloatingPoint (Text) HexBytes";

#ifdef __cplusplus_cli
void values32b_to_cout(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
System::String^ values32b_to_clistr(System::String^ prefix, BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
#endif
