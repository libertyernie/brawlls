#include "brawlextract.h"

using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

int extract(ResourceNode^ node, String^ filename) {
	if (String::IsNullOrEmpty(filename)) {
		Console::Error->WriteLine("x: no arguments specified");
		Console::Error->WriteLine("Use [... x filename] or [... x .extension]");
		return 1;
	} else if (filename->StartsWith(".")) {
		filename = node->Name + filename;
	}
	node->Export(filename);
	return 0;
}

int extract_all(ResourceNode^ parent, String^ dirname, String^ ext) {
	if (String::IsNullOrEmpty(dirname)) {
		Console::Error->WriteLine("xall: no arguments specified");
		return 1;
	} else if (parent->Children->Count == 0) {
		Console::Error->WriteLine("Cannot extract: node " + parent->Name + " has no children.");
		return 1;
	} else if (Directory::Exists(dirname)) {
		for each(ResourceNode^ node in parent->Children) {
			String^ p = dirname + Path::DirectorySeparatorChar + node->Name;
			if (!String::IsNullOrEmpty(ext)) p += "." + ext;
			Console::WriteLine("Extracting " + node->Name + " to " + p + "...");
			node->Export(p);
		}
	} else {
		Console::Error->WriteLine("Cannot extract: directory " + dirname + " does not exist.");
		return 1;
	}
}
