// For easier access from within C++
System::String^ md5str(void* addr, int len);
System::String^ md5str(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);

// For visibility in other languages (C#, Visual Basic) if needed
namespace BrawlLS {
	public ref class Methods {
	public:
		static System::String^ MD5Str(void* addr, int len);
		static System::String^ MD5Str(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
	};
}
