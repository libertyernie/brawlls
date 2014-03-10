#include "brawlmd5.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Text::RegularExpressions;
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

template < class T, class U >
Boolean isinst(U u) {
	return dynamic_cast< T >(u) != nullptr;
}

void find_children_recursive(ResourceNode^ root, String^ nodepath, List<ResourceNode^>^ output);
void print_recursive(String^ prefix, ResourceNode^ node, bool isRoot, int maxdepth);
void printf_obj(String^ format, String^ prefix, Object^ obj);
void print_obj(String^ prefix, Object^ obj);
void print_properties(String^ prefix, ResourceNode^ node);

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
	List<ResourceNode^> matchingNodes;
	if (nodepath == nullptr) {
		matchingNodes.Add(node);
	} else if (searchChildren) {
		matchingNodes.Add(node->FindChild(nodepath, true));
	} else {
		nodepath = nodepath->Replace("+", "/+")->Replace("//", "/");
		while (nodepath->StartsWith("/")) {
			nodepath = nodepath->Substring(1);
		}
		find_children_recursive(node, nodepath, %matchingNodes);
	}

	if (matchingNodes.Count == 0) return usage("No nodes found matching path: " + nodepath);
	
	if (printSelf) {
		for each(ResourceNode^ child in matchingNodes) {
			//Console::WriteLine(child->Name);
			printf_obj("%i %n", "", child);
		}
	} else if (matchingNodes.Count == 1) {
		int maxdepth = recursive ? -1 : 1;
		print_recursive("", matchingNodes[0], true, maxdepth);
	} else {
		Console::Error->WriteLine("Error: search matched more than one node. Use --self to list them.");
		return 1;
	}
}

void find_children_recursive(ResourceNode^ root, String^ nodepath, List<ResourceNode^>^ output) {
	if (String::IsNullOrWhiteSpace(nodepath)) {
		output->Add(root);
		return;
	}

	int slashat = nodepath->IndexOf('/');
	String^ pattern = slashat < 0
		? nodepath
		: nodepath->Substring(0, slashat);
	String^ remainder = slashat < 0
		? ""
		: nodepath->Substring(slashat + 1);

	Int32 abs_index;
	if (pattern->StartsWith("+") && Int32::TryParse(pattern->Substring(1), abs_index)) {
		find_children_recursive(root->Children[abs_index], remainder, output);
	} else {
		String^ rstr = "^" + Regex::Escape(pattern)
			->Replace("\\*", ".*")
			->Replace("\\?", ".") + "$";
		Regex^ exp = gcnew Regex(rstr);
		for each(ResourceNode^ child in root->Children) {
			if (exp->IsMatch(child->Name)) {
				find_children_recursive(child, remainder, output);
			}
		}
	}
}

void print_recursive(String^ prefix, ResourceNode^ node, bool isRoot, int maxdepth) {
	if (!isRoot) {
		print_obj(prefix, node);
		//Console::WriteLine(prefix + node->Name);
		prefix += "  ";
	}

	if (maxdepth == 0) return;

	if (isinst<STPMEntryNode^>(node) && stpmValues) {
		print_properties(prefix, node);
	} else {
		if (isinst<MDL0Node^>(node)) {
			if (modelsDeep == NEVER) {
				return;
			} else if (modelsDeep == SELECTIVE && !isRoot && !node->Name->EndsWith("osition")) {
				return;
			}
		}

		int newdepth = maxdepth < 0
			? -1
			: maxdepth - 1;
		for each(ResourceNode^ child in node->Children) {
			print_recursive(prefix, child, false, newdepth);
		}
	}
}

void printf_obj(String^ format, String^ prefix, Object^ obj) {
	String^ name = obj == nullptr
		? "null"
		: obj->ToString();
	String^ index = isinst<ResourceNode^>(obj)
		? ((ResourceNode^)obj)->Index + ""
		: "";
	String^ bone = "";
	if (isinst<MDL0BoneNode^>(obj)) {
		MDL0BoneNode^ b = (MDL0BoneNode^)obj;
		bone = "T" + b->Translation;
		bone += " R" + b->Rotation;
		bone += " S" + b->Scale;
	}
	String^ md5;
	if (isinst<ResourceNode^>(obj)) {
		ResourceNode^ node = (ResourceNode^)obj;
		md5 = "MD5:" + md5str(node);
	}
	String^ line = format
		->Replace("%p", prefix)
		->Replace("%n", name)
		->Replace("%i", index)
		->Replace("%t", "(" + obj->GetType()->Name + ")")
		->Replace("%b", bone)
		->Replace("%m", md5)
		->Replace("%%", "%");
	Console::WriteLine(line);
}

void print_obj(String^ prefix, Object^ obj) {
	printf_obj("%p%i %n" +
		(showtype ? " %t" : "") +
		" %b" +
		(printMD5 ? " %m" : ""),
		prefix, obj);
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