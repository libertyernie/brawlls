#include "brawlextract.h"

using namespace System;
using namespace System::IO;
using System::Collections::Generic::List;
using BrawlLib::SSBB::ResourceNodes::ResourceNode;

const char* xall_help = R"(Usage: brawlls [OPTIONS] FILENAME PATH xall OUTPUT-DIR [extension]

The xall command extracts all direct children of a node within FILENAME,
given by PATH, to the directory OUTPUT-DIR.

The -c option is the only brawlls option that can be used with xall.

Elements of the inside-file path can be node names, with or without
wildcards (*), or indicies prefixed by "+" (for example, +0 or +17).
The + character does not need to be preceded by a slash.

Example:
mkdir outdir
brawlls common5.pac sc_selmap_en/*80]/Tex* # lists all textures
brawlls common5.pac sc_selmap_en/*80]/Tex* xall outdir png # extract to folder)";


int extract_all(ResourceNode^ parent, List<String^>^ args) {
	if (args->Count == 0) {
		Console::Error->WriteLine(gcnew String(xall_help));
		return 1;
	} else if (args->Contains("--help") || args->Contains("/?")) {
		Console::WriteLine(gcnew String(xall_help));
		return 0;
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
