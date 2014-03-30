int extract(
	BrawlLib::SSBB::ResourceNodes::ResourceNode^ node,
	System::String^ filename);
int extract_all(
	BrawlLib::SSBB::ResourceNodes::ResourceNode^ parent,
	System::String^ dirname, System::String^ ext);
int extract_values32b(
	BrawlLib::SSBB::ResourceNodes::ResourceNode^ node,
	System::String^ filename);

