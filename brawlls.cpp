#include <iostream>

using namespace System;
using namespace System::IO;
using namespace BrawlLib::SSBB::ResourceNodes;

const char* usage_line = "Usage: brawlls [options] filename [path within file]";
const char* usage_help_line = "Run with --help or /? for more information.";

const char* usage_desc = R"(
    -d  list only the specified node(s), not their children (akin to ls -d)
    -R  list nodes recursively (akin to ls -R)

    -t  show node types next to names
    -m  show MD5 checksums next to names
    -c  find first child matching path (disables wildcards and + prefixes)

    --self     alias for -d
    --help     print this message to stdout
    /?         print this message to stdout

    --stpm     list STPM values as children (default)
    --no-stpm  don't list STPM values
    --bone     print values of MDL0 bones on same line (default)
    --no-bone  don't print bone values
    --mdl0     always list sub-nodes of MDL0 models
    --no-mdl0  never list sub-nodes of MDL0 models
    The default is to show MDL0 sub-nodes when the node is the working root
    (specified on the command-line) or its name ends in "osition".

    Elements of the inside-file path can be node names, with or without
    wildcards (*), or indicies prefixed by "+" (for example, +0 or +17).
    The + character does not need to be preceded by a slash.)";

int usage(String^ error_msg) {
	if (error_msg->Length != 0) Console::Error->WriteLine(error_msg + "\n");
	Console::Error->WriteLine(gcnew String(usage_line));
	Console::Error->WriteLine(gcnew String(usage_help_line));
	return 1;
}

typedef enum {
	ALWAYS, NEVER, SELECTIVE
} MDL0PrintType;

bool recursive = false, // -R
stpmValues = true, // --stpm, --no-stpm
boneValues = true, // --bone, --no-bone
showtype = false, // -t
printSelf = false, // -d, --self
searchChildren = false, // -c
printMD5 = false; // -m
MDL0PrintType modelsDeep = SELECTIVE; // --mdl0, --no-mdl0

int main(array<System::String ^> ^args) {
	if (args->Length == 0) {
		return usage("");
	}
	//Console::WriteLine(IO::Path::GetFileName(Reflection::Assembly::GetEntryAssembly()->Location));

	String^ filename;
	String^ nodepath;
	for each(String^ argument in args) {
		if (argument == "--help" || argument == "/?") {
			Console::WriteLine(gcnew String(usage_line));
			Console::WriteLine(gcnew String(usage_desc));
			return 0;
		}
		if (argument == "--self") printSelf = true;
		else if (argument == "--stpm") stpmValues = true;
		else if (argument == "--no-stpm") stpmValues = false;
		else if (argument == "--bone") boneValues = true;
		else if (argument == "--no-bone") boneValues = false;
		else if (argument == "--mdl0") modelsDeep = ALWAYS;
		else if (argument == "--no-mdl0") modelsDeep = NEVER;
		else if (argument->StartsWith("-")) {
			for each(char c in argument->Substring(1)) {
				if (c == 'd') printSelf = true;
				else if (c == 'R') recursive = true;
				else if (c == 't') showtype = true;
				else if (c == 'm') printMD5 = true;
				else if (c == 'c') searchChildren = true;
				else {
					return usage("Invalid argument: " + argument);
				}
			}
		} else {
			if (nodepath != nullptr) {
				return usage("Error: too many arguments: " + filename + " " + nodepath + " " + argument);
			} else if (filename != nullptr) {
				nodepath = argument;
			} else {
				filename = argument;
			}
		}
	}

	if (filename == nullptr) return usage("Error: no filename given.");
	if (!File::Exists(filename)) return usage("Error: file not found: " + filename);

	ResourceNode^ node = NodeFactory::FromFile(nullptr, filename);
	Console::WriteLine(node->Name);
	delete node;
}
