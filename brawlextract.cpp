using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

int extract(List<ResourceNode^>^ nodes, String^ path) {
	if (path == nullptr) {
		Console::Error->WriteLine("Error: no output file specified");
		return 1;
	} else if (Directory::Exists(path)) {
		for each(ResourceNode^ node in nodes) {
			String^ p = path + Path::DirectorySeparatorChar + node->Name;
			Console::WriteLine(p);
			node->Export(p);
		}
	} else if (nodes->Count > 1) {
		Console::Error->WriteLine("Error: cannot extract multiple nodes to single file");
		return 1;
	} else {
		nodes[0]->Export(path);
	}
}