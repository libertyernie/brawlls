#include <cstring>
#include <cstdlib>
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

enum class ProgramBehavior {
	UNDEFINED, NORMAL, EXTRACT, EXTRACT_ALL, PRINT_VALUES
};

void print_recursive(String^ format, String^ prefix, ResourceNode^ node, bool deep, bool mdl0deep, bool isRoot, int maxdepth) {
	if (!isRoot) {
		Console::WriteLine(format_obj(format, prefix, node));
		prefix += "  ";
	}

	if (maxdepth == 0) return;

	String^ details = deep ? details_str(prefix, node) : nullptr;
	if (details != nullptr) {
		Console::Write(details);
	} else {
		if (isinst<BRESEntryNode^>(node) && !isRoot && !mdl0deep) return;

		int newdepth = maxdepth < 0
			? -1
			: maxdepth - 1;
		for each(ResourceNode^ child in node->Children) {
			print_recursive(format, prefix, child, deep, mdl0deep, false, newdepth);
		}
	}
	delete node; // calls Dispose(). this may improve performance slightly, but we can't use these nodes later in the program
}

ref struct cleanup_args {
	static ResourceNode^ tempFile;
};

void cleanup() {
	if (cleanup_args::tempFile != nullptr) {
		String^ path = cleanup_args::tempFile->FilePath;
		delete cleanup_args::tempFile;
		File::Delete(path);
	}
}

void stdin_to_file(String^ tmpfile) {
	Stream^ clistdin = Console::OpenStandardInput();
	Stream^ outstream = gcnew FileStream(tmpfile, FileMode::Create, FileAccess::Write);
	array<unsigned char>^ buffer = gcnew array<unsigned char>(2048);
	int bytes;
	while ((bytes = clistdin->Read(buffer, 0, buffer->Length)) > 0) {
		outstream->Write(buffer, 0, bytes);
	}
	outstream->Close();
}

ResourceNode^ load_node_from_file_or_stdin(String^ filename) {
	ResourceNode^ node;
	if (filename != "-") {
		node = NodeFactory::FromFile(nullptr, filename, System::IO::FileOptions::RandomAccess);
	} else {
		// write contents of stdin to a temporary file
		String^ tmpfile = Path::GetTempFileName();
		stdin_to_file(tmpfile);
		node = NodeFactory::FromFile(nullptr, tmpfile, System::IO::FileOptions::RandomAccess);
		cleanup_args::tempFile = node;
		atexit(cleanup);
	}
	return node;
}

int brawlls(array<String^>^ args) {
	if (args->Length == 0) {
		return usage("");
	}

	// most basic arguments
	String^ filename;
	String^ nodepath;
	String^ format;

	// affects printout only
	bool recursive = false, // -R
		boneValues = true, // --bone, --no-bone
		showtype = false, // -t
		printSelf = false, // -d, --self
		printMD5 = false, // -m
		fullpath = false; // --full-path

	// affects node search
	bool searchChildren = false; // -c
	bool deep = false; // --deep
	bool mdl0deep = false; // --mdl0

	// other arguments
	ProgramBehavior behavior = ProgramBehavior::UNDEFINED;
	List<String^> behavior_arguments; // arguments not otherwise defined that come after the behavior

	for each(String^ argument in args) {
		if (argument == "--help" || argument == "/?") {
			Console::WriteLine(gcnew String(title));
			Console::WriteLine(gcnew String(usage_line));
			Console::WriteLine(gcnew String(usage_desc));
			return 0;
		} else if (argument == "--formathelp") {
			Console::WriteLine(gcnew String(format_help));
			return 0;
		} else if (argument == "--xallhelp") {
			Console::WriteLine(gcnew String(xall_help));
			return 0;
		} else if (argument == "--valshelp") {
			Console::WriteLine(gcnew String(vals_help));
			return 0;
		} else if (argument == "--license") {
			Console::WriteLine(gcnew String(LICENSE));
			return 0;
		}
		if (argument == "--self") printSelf = true;
		else if (argument == "--full-path") fullpath = true;
		else if (argument == "--deep") deep = true;
		else if (argument == "--mdl0") mdl0deep = true;
		else if (argument == "--shallow") deep = false;
		else if (argument == "--bone") boneValues = true;
		else if (argument == "--no-bone") boneValues = false;
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
			else if (behavior == ProgramBehavior::UNDEFINED && argument == "vals") behavior = ProgramBehavior::PRINT_VALUES;
			else if (behavior != ProgramBehavior::UNDEFINED) behavior_arguments.Add(argument);
			else if (nodepath == nullptr) nodepath = argument;
			else return usage("Error: too many arguments: " + filename + " " + nodepath + " " + argument);
		}
	}
	if (behavior == ProgramBehavior::UNDEFINED) behavior = ProgramBehavior::NORMAL;
	if (deep || mdl0deep) recursive = true;

	if (filename == nullptr) return usage("Error: no filename given.");
	if (filename != "-" && !File::Exists(filename)) return usage("Error: file not found: " + filename);

	// fill in format value, unless overridden
	if (format == nullptr) {
		format = "%~+%i" +
			(fullpath ? " %p" : " %n") +
			(showtype ? " %t" : "") +
			(boneValues ? " %b" : "") +
			(printMD5 ? " %m" : "");
	}

	// load resource node
	ResourceNode^ node = load_node_from_file_or_stdin(filename);

	List<ResourceNode^>^ matchingNodes = find_children(node, nodepath, searchChildren);

	if (matchingNodes->Count == 0) return usage("No nodes found matching path: " + nodepath);

	// handle --self/-d
	// (but ignore --self/-d when using brawlls to extract something)
	// Do this here, because it's the only method of the program that can handle >1 matching node
	if (behavior == ProgramBehavior::NORMAL && printSelf) {
		for each(ResourceNode^ child in matchingNodes) {
			Console::WriteLine(format_obj(format, "", child));
		}
		return 0;
	}

	if (matchingNodes->Count > 1) {
		Console::Error->WriteLine("Search matched " + matchingNodes->Count + " nodes. Use -d or --self to list them.");
		return 1;
	}
	
	if (behavior == ProgramBehavior::EXTRACT) {
		String^ filename = behavior_arguments.Count >= 1 ? behavior_arguments[0] : nullptr;
		if (filename == nullptr) {
			Console::Error->WriteLine("Error: no output filename or extension specified");
			Console::Error->WriteLine("Use - as an output filename to extract to stdout");
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
	} else if (behavior == ProgramBehavior::PRINT_VALUES) {
		String^ filename = behavior_arguments.Count >= 1 ? behavior_arguments[0] : nullptr;
		if (filename == nullptr) {
			Console::Error->WriteLine("Error: no output filename or extension specified");
			Console::Error->WriteLine("Use - as an output filename to extract to stdout");
			Console::Error->WriteLine(gcnew String(vals_help));
			return 1;
		}
		return extract_values32b(matchingNodes[0], filename);
	}
	
	// attempt recursive printout of children (not including self)
	String^ details = details_str("", matchingNodes[0]);
	if (details != nullptr) {
		Console::Write(details);
		return 0;
	} else if (matchingNodes[0]->Children->Count == 0) {
		Console::Error->WriteLine("The node " + matchingNodes[0]->Name + " does not have any children.");
		return 0;
	} else {
		int maxdepth = recursive ? -1 : 1;
		print_recursive(format, "", matchingNodes[0], deep, mdl0deep, true, maxdepth);
		return 0;
	}
}

int main(array<String^>^ args) {
	return brawlls(args);
}
