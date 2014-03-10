void find_children(BrawlLib::SSBB::ResourceNodes::ResourceNode^ root,
	System::String^ nodepath,
	System::Collections::Generic::List<BrawlLib::SSBB::ResourceNodes::ResourceNode^>^ output,
	bool useBrawlLibSearchChildren);
void find_children_recursive(BrawlLib::SSBB::ResourceNodes::ResourceNode^ root,
	System::String^ nodepath,
	System::Collections::Generic::List<BrawlLib::SSBB::ResourceNodes::ResourceNode^>^ output);
