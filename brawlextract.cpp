#include "brawlextract.h"

using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

int extract_all(ResourceNode^ parent, List<String^>^ args) {
	return extract(parent->Children, args);
}

int extract(List<ResourceNode^>^ nodes, List<String^>^ args) {
	if (args->Count == 0) {
		Console::Error->WriteLine("Error: no output file specified");
		return 1;
	} else if (Directory::Exists(args[0])) {
		for each(ResourceNode^ node in nodes) {
			String^ p = args[0] + Path::DirectorySeparatorChar + node->Name;
			if (args->Count >= 2) p += "." + args[1];
			node->Export(p);
		}
	} else if (nodes->Count > 1) {
		Console::Error->WriteLine("Error: cannot extract multiple nodes to single file.");
		Console::Error->WriteLine("(" + args[0] + " is not a directory - do you need to create it?)");
		return 1;
	} else if (nodes->Count == 0) {
		Console::Error->WriteLine("Error: no nodes to extract");
		return 1;
	} else {
		nodes[0]->Export(args[0]);
	}
}