#include "brawlmd5.h"
#include "brawlextract.h"
#include "find_children.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Text::RegularExpressions;
using namespace BrawlLib::SSBB::ResourceNodes;
using BrawlLS::MD5;

const char* usage_line = "Usage: brawlls [options] filename [path within file]";
const char* usage_help_line = "Run with --help or /? for more information.";

const char* usage_desc = R"(
-d, --self  list only the specified nodes, not their children (akin to ls -d)
-R          list nodes recursively (akin to ls -R)
-c          find only the first path that matches (disables wildcards and +s)
--help, /?  print this message to stdout
--xallhelp  info about the "xall" command

--no-stpm  don't list STPM values (default is --stpm)
--no-bone  don't print bone values (default is --bone)
--mdl0     always list sub-nodes of MDL0 models
--no-mdl0  never list sub-nodes of MDL0 models
The default is to show MDL0 sub-nodes when the node is the working root
(specified on the command-line) or its name ends in "osition".

-t              show node types next to names
-m              show MD5 checksums next to names
--full-path     show full path instead of name
--format="..."  overrides -t, -m, --bone, --no-bone, and --full-path;
                run "brawlls --formathelp" for more information

Elements of the inside-file path can be node names, with or without
wildcards (*), or indicies prefixed by "+" (for example, +0 or +17).
The + character does not need to be preceded by a slash.)";

const char* format_help = R"(
Default format with -t and -m: %~+%i %n %t %b %m
%t appears when -t is specified, %m appears for -m, and %b
appears by default (but can be turned off with --no-bone.)
    
Available options for --format:
%~  indentation for recursive listing (two spaces for each depth)
%i  index of node in parent (you can use this with + prefix in pathname)
%n  name of node
%p  full path of node inside file
%t  type of node
%b  trans/rot/scale of MDL0 bones (only appears for MDL0BoneNodes)
%m  MD5 checksum of original node data
%s  size in bytes of original node data
%%  literal % sign)";

int usage(String^ error_msg) {
	if (error_msg->Length != 0) Console::Error->WriteLine(error_msg + "\n");
	Console::Error->WriteLine(gcnew String(usage_line));
	Console::Error->WriteLine(gcnew String(usage_help_line));
	return 1;
}

enum class MDL0PrintType {
	ALWAYS, NEVER, SELECTIVE
};

enum class ProgramBehavior {
	UNDEFINED, NORMAL, EXTRACT_ALL
};

template < class T, class U >
Boolean isinst(U u) {
	return dynamic_cast< T >(u) != nullptr;
}

void print_recursive(String^ format, String^ prefix, ResourceNode^ node, MDL0PrintType modelsDeep, bool stpmValues, bool isRoot, int maxdepth);
void printf_obj(String^ format, String^ prefix, Object^ obj);
void print_properties(String^ prefix, ResourceNode^ node);

int main(array<System::String ^> ^args) {
	if (args->Length == 0) {
		return usage("");
	}

	String^ filename;
	String^ nodepath;
	String^ format;
	MDL0PrintType modelsDeep = MDL0PrintType::SELECTIVE; // --mdl0, --no-mdl0

	// affects printout only
	bool recursive = false, // -R
		stpmValues = true, // --stpm, --no-stpm
		boneValues = true, // --bone, --no-bone
		showtype = false, // -t
		printSelf = false, // -d, --self
		printMD5 = false, // -m
		fullpath = false; // --full-path

	// affects node search
	bool searchChildren = false; // -c

	ProgramBehavior behavior = ProgramBehavior::UNDEFINED;
	List<String^> behavior_arguments; // arguments not otherwise defined that come after the behavior

	for each(String^ argument in args) {
		if (argument == "--help" || argument == "/?") {
			Console::WriteLine(gcnew String(usage_line));
			Console::WriteLine(gcnew String(usage_desc));
			return 0;
		}
		if (argument == "--formathelp") {
			Console::WriteLine(gcnew String(format_help));
			return 0;
		}
		if (argument == "--xallhelp") {
			List<String^> l;
			l.Add("--help");
			return extract_all(nullptr, %l);
		}
		if (argument == "--self") printSelf = true;
		else if (argument == "--stpm") stpmValues = true;
		else if (argument == "--no-stpm") stpmValues = false;
		else if (argument == "--bone") boneValues = true;
		else if (argument == "--no-bone") boneValues = false;
		else if (argument == "--mdl0") modelsDeep = MDL0PrintType::ALWAYS;
		else if (argument == "--no-mdl0") modelsDeep = MDL0PrintType::NEVER;
		else if (argument == "--full-path") fullpath = true;
		else if (argument->StartsWith("--format=")) format = argument->Substring(9);
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
			if (filename == nullptr) filename = argument;
			else if (nodepath == nullptr) nodepath = argument;
			else if (behavior == ProgramBehavior::UNDEFINED && argument == "xall") behavior = ProgramBehavior::EXTRACT_ALL;
			else if (behavior != ProgramBehavior::UNDEFINED) behavior_arguments.Add(argument);
			else return usage("Error: too many arguments: " + filename + " " + nodepath + " " + argument);
		}
	}
	if (behavior == ProgramBehavior::UNDEFINED) behavior = ProgramBehavior::NORMAL;

	if (filename == nullptr) return usage("Error: no filename given.");
	if (!File::Exists(filename)) return usage("Error: file not found: " + filename);

	if (format == nullptr) {
		format = "%~+%i" +
			(fullpath ? " %p" : " %n") +
			(showtype ? " %t" : "") +
			(boneValues ? " %b" : "") +
			(printMD5 ? " %m" : "");
	}

	ResourceNode^ node = NodeFactory::FromFile(nullptr, filename);
	List<ResourceNode^> matchingNodes;
	find_children(node, nodepath, %matchingNodes, searchChildren);

	if (matchingNodes.Count == 0) return usage("No nodes found matching path: " + nodepath);

	if (behavior == ProgramBehavior::NORMAL && printSelf) {
		for each(ResourceNode^ child in matchingNodes) {
			printf_obj(format, "", child);
		}
	} else if (matchingNodes.Count > 1) {
		Console::Error->WriteLine("Search matched " + matchingNodes.Count + " nodes. Use -d or --self to list them.");
		return 1;
	} else if (behavior == ProgramBehavior::EXTRACT_ALL) {
		return extract_all(matchingNodes[0], %behavior_arguments);
	} else if (matchingNodes[0]->Children->Count == 0) {
		Console::Error->WriteLine("The node " + matchingNodes[0]->Name + " does not have any children.");
		return 0;
	} else {
		int maxdepth = recursive ? -1 : 1;
		print_recursive(format, "", matchingNodes[0], modelsDeep, stpmValues, true, maxdepth);
	}
}

void print_recursive(String^ format, String^ prefix, ResourceNode^ node, MDL0PrintType modelsDeep, bool stpmValues, bool isRoot, int maxdepth) {
	if (!isRoot) {
		printf_obj(format, prefix, node);
		prefix += "  ";
	}

	if (maxdepth == 0) return;

	if (isinst<STPMEntryNode^>(node) && stpmValues) {
		print_properties(prefix, node);
	} else {
		if (isinst<MDL0Node^>(node)) {
			if (modelsDeep == MDL0PrintType::NEVER) {
				return;
			} else if (modelsDeep == MDL0PrintType::SELECTIVE && !isRoot && !node->Name->EndsWith("osition")) {
				return;
			}
		}

		int newdepth = maxdepth < 0
			? -1
			: maxdepth - 1;
		for each(ResourceNode^ child in node->Children) {
			print_recursive(format, prefix, child, modelsDeep, stpmValues, false, newdepth);
		}
	}
	delete node; // calls Dispose(). this may improve performance slightly, but we can't use these nodes later in the program
}

void printf_obj(String^ format, String^ prefix, Object^ obj) {
	String^ name = obj == nullptr
		? "null"
		: obj->ToString();
	String^ bone = "";
	if (isinst<MDL0BoneNode^>(obj)) {
		MDL0BoneNode^ b = (MDL0BoneNode^)obj;
		bone = "T" + b->Translation;
		bone += " R" + b->Rotation;
		bone += " S" + b->Scale;
	}
	String^ index = "";
	String^ md5 = "";
	String^ size = "";
	String^ path = "";
	if (isinst<ResourceNode^>(obj)) {
		ResourceNode^ node = (ResourceNode^)obj;
		if (format->Contains("%m")) {
			// don't do this if we don't need the data - this does save some time
			md5 = "MD5:" + MD5::MD5Str(node);
		}
		index = node->Index + "";
		size = node->OriginalSource.Length + "";
		while (node->Parent != nullptr) {
			path = node->Name + "/" + path;
			node = node->Parent;
		}
		if (path->EndsWith("/")) path = path->Substring(0, path->Length - 1);
	}
	String^ line = format
		->Replace("%~", prefix)
		->Replace("%n", name)
		->Replace("%p", path)
		->Replace("%i", index)
		->Replace("%t", "(" + obj->GetType()->Name + ")")
		->Replace("%b", bone)
		->Replace("%m", md5)
		->Replace("%s", size)
		->Replace("%%", "%");
	Console::WriteLine(line);
}

void print_properties(String^ prefix, ResourceNode^ node) {
	for each(PropertyInfo^ entry in node->GetType()->GetProperties()) {
		for each(Attribute^ attribute in entry->GetCustomAttributes(false)) {
			if (isinst<CategoryAttribute^>(attribute)) {
				Object^ val = entry->GetValue(node, nullptr);
				String^ valstr = val == nullptr
					? "null"
					: val->ToString();
				Console::WriteLine(prefix + entry->Name + " " + valstr);
			}
		}
	}
}
