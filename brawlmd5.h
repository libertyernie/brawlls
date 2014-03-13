namespace BrawlLS {
	public ref class MD5 {
	public:
		static System::String^ MD5Str(void* addr, int len);
		static System::String^ BrawlLS::MD5::MD5Str(System::String^ pathname);
		static System::String^ MD5Str(BrawlLib::SSBB::ResourceNodes::ResourceNode^ node);
		static System::String^ BrawlLS::MD5::MD5Str(System::Collections::Generic::IEnumerable<BrawlLib::SSBB::ResourceNodes::ResourceNode^>^ nodes);
	};
}
