#include "brawlextract.h"
#include "values32b.h"
#include <cstdio>
#include <fcntl.h>
#include <io.h>

using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

int extract(ResourceNode^ node, String^ filename) {
	if (String::IsNullOrEmpty(filename)) {
		Console::Error->WriteLine("x: no arguments specified");
		Console::Error->WriteLine("Use [... x filename], [... x .extension] or [... x -]");
		return 1;
	}

	if (filename == "-") {
		Console::Error->WriteLine("Note: exporting uncompressed source - in certain cases (e.g. the BRES entry header), this may differ from what would be written to a file.");
		_setmode(_fileno(stdout), O_BINARY);
		fwrite(node->UncompressedSource.Address, 1, node->UncompressedSource.Length, stdout);
		return 0;
	}

	if (filename->StartsWith(".")) {
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
		return 0;
	} else {
		Console::Error->WriteLine("Cannot extract: directory " + dirname + " does not exist.");
		return 1;
	}
}

int extract_values32b(ResourceNode^ node, String^ filename) {
	if (String::IsNullOrEmpty(filename)) {
		Console::Error->WriteLine("values: no arguments specified");
		Console::Error->WriteLine("Use [... values filename], [... values .extension] or [... values -]");
		return 1;
	}

	if (filename == "-") {
		Console::Error->WriteLine("Note: reading uncompressed source - in certain cases (e.g. the BRES entry header), this may differ from what would be written to a file.");
		values32b_to_cout(node->UncompressedSource.Address, node->UncompressedSource.Length);
		return 0;
	}

	if (filename->StartsWith(".")) {
		filename = node->Name + filename;
	}
	File::WriteAllText(filename, values32b_to_clistr("", node));
	return 0;
}