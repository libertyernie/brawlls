#include "brawlextract.h"

using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

int extract_all(ResourceNode^ parent, List<String^>^ args) {
	if (args->Count == 0) {
		Console::Error->WriteLine("Cannot extract: no output folder specified.");
		return 1;
	} else if (parent->Children->Count == 0) {
		Console::Error->WriteLine("Cannot extract: node " + parent->Name + " has no children.");
		return 1;
	} else if (Directory::Exists(args[0])) {
		for each(ResourceNode^ node in parent->Children) {
			String^ p = args[0] + Path::DirectorySeparatorChar + node->Name;
			if (args->Count >= 2) p += "." + args[1];
			Console::WriteLine("Extracting " + node->Name + " to " + p + "...");
			node->Export(p);
		}
	} else {
		Console::Error->WriteLine("Cannot extract: directory " + args[0] + " does not exist.");
		return 1;
	}
}
