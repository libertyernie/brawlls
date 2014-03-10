namespace BrawlLS {
	public ref class Methods {
	public:
		static System::String^ MD5Str(void* addr, int len);
		static System::String^ MD5Str(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
	};
}
