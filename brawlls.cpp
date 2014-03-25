#include "brawlextract.h"
#include "brawlprintf.h"
#include "find_children.h"
#include "usage.h"
#include "isinst.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace BrawlLib::SSBB::ResourceNodes;

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
	UNDEFINED, NORMAL, EXTRACT, EXTRACT_ALL
};



void print_recursive(String^ format, String^ prefix, ResourceNode^ node, MDL0PrintType modelsDeep, bool stpmValues, bool isRoot, int maxdepth) {
	if (!isRoot) {
		Console::WriteLine(format_obj(format, prefix, node));
		prefix += "  ";
	}

	if (maxdepth == 0) return;

	if (isinst<STPMEntryNode^>(node) && stpmValues) {
		Console::Write(properties_str(prefix, node));
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

int brawlls(array<String^>^ args) {
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
			Console::WriteLine(gcnew String(xall_help));
			return 0;
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
		else if (argument->StartsWith("-") && argument->Length > 1) {
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
			else if (behavior == ProgramBehavior::UNDEFINED && argument == "x") behavior = ProgramBehavior::EXTRACT;
			else if (behavior == ProgramBehavior::UNDEFINED && argument == "xall") behavior = ProgramBehavior::EXTRACT_ALL;
			else if (behavior != ProgramBehavior::UNDEFINED) behavior_arguments.Add(argument);
			else if (nodepath == nullptr) nodepath = argument;
			else return usage("Error: too many arguments: " + filename + " " + nodepath + " " + argument);
		}
	}
	if (behavior == ProgramBehavior::UNDEFINED) behavior = ProgramBehavior::NORMAL;

	if (filename == "-") {
		String^ tmpfile = Path::GetTempFileName();
		Stream^ stdin = Console::OpenStandardInput();
		Stream^ outstream = gcnew FileStream(tmpfile, FileMode::Create, FileAccess::Write);
		array<unsigned char>^ buffer = gcnew array<unsigned char>(2048);
		int bytes;
		while ((bytes = stdin->Read(buffer, 0, buffer->Length)) > 0) {
			outstream->Write(buffer, 0, bytes);
		}
		outstream->Close();
		filename = tmpfile;
	}

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
			Console::WriteLine(format_obj(format, "", child));
		}
		return 0;
	} else if (matchingNodes.Count > 1) {
		Console::Error->WriteLine("Search matched " + matchingNodes.Count + " nodes. Use -d or --self to list them.");
		return 1;
	} else if (behavior == ProgramBehavior::EXTRACT) {
		String^ filename = behavior_arguments.Count >= 1 ? behavior_arguments[0] : nullptr;
		if (filename == nullptr) {
			Console::Error->WriteLine("Error: no output filename or extension specified");
			Console::Error->WriteLine(gcnew String(x_help));
			return 1;
		}
		return extract(matchingNodes[0], filename);
	} else if (behavior == ProgramBehavior::EXTRACT_ALL) {
		String^ dir = behavior_arguments.Count >= 1 ? behavior_arguments[0] : nullptr;
		String^ ext = behavior_arguments.Count >= 2 ? behavior_arguments[1] : nullptr;
		if (dir == nullptr) {
			Console::Error->WriteLine("Error: no output directory specified");
			Console::Error->WriteLine(gcnew String(xall_help));
			return 1;
		}
		return extract_all(matchingNodes[0], dir, ext);
	} else if (isinst<STPMEntryNode^>(matchingNodes[0])) {
		Console::Write(properties_str("", matchingNodes[0]));
		return 0;
	} else if (matchingNodes[0]->Children->Count == 0) {
		Console::Error->WriteLine("The node " + matchingNodes[0]->Name + " does not have any children.");
		return 0;
	} else {
		int maxdepth = recursive ? -1 : 1;
		print_recursive(format, "", matchingNodes[0], modelsDeep, stpmValues, true, maxdepth);
		return 0;
	}
}

