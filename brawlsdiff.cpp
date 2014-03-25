#include "brawlprintf.h"
#include "brawlsdiff.h"
#include "isinst.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::IO;
using namespace BrawlLib::SSBB::ResourceNodes;

const char* brawlsdiff_usage = R"(Usage: brawlls sdiff [options] filename1 filename2

-s              only show differences; don't print matching nodes

If you're using Cygwin, it probably makes more sense to use the brawlsdiff.sh
script to feed brawlls's output to your own sdiff program. brawlsdiff.sh can
read Unix paths, has a -w [width] option, and has all the options of brawlls,
including descending into STPM nodes. This internal "sdiff" mode of brawlls
will probably not be expanded any further.
)";

const int colwidth = 78;

int compare(ResourceNode^ left, ResourceNode^ right, bool differences_only, String^ prefix) {
	String^ leftstr = (left == nullptr) ? "" : format_obj("%~+%i %n %t %b %m", prefix, left);
	if (leftstr->Length > colwidth) leftstr = leftstr->Substring(0, colwidth);
	String^ rightstr = (right == nullptr) ? "" : format_obj("%~+%i %n %t %b %m", prefix, right);
	if (rightstr->Length > colwidth) rightstr = rightstr->Substring(0, colwidth);

	StringBuilder leftprint(leftstr);
	while (leftprint.Length < colwidth) leftprint.Append(" ");
	StringBuilder rightprint(rightstr);
	while (rightprint.Length < colwidth) rightprint.Append(" ");

	wchar_t sepchar = left == nullptr ? '>'
		: right == nullptr ? '<'
		: leftstr == rightstr ? ' '
		: '|';

	StringBuilder line;
	line.Append(%leftprint);
	line.Append(" ");
	line.Append(sepchar);
	line.Append(" ");
	line.Append(%rightprint);
	if (!differences_only||sepchar != ' ') Console::WriteLine(%line);

	prefix += "  ";

	List<ResourceNode^>^ leftchildren = (left == nullptr) ? gcnew List<ResourceNode^>() : left->Children;
	List<ResourceNode^>^ rightchildren = (right == nullptr) ? gcnew List<ResourceNode^>() : right->Children;
	int maxct = Math::Max(leftchildren->Count, rightchildren->Count);
	for (int i = 0; i < maxct; i++) {
		int result = compare(
			leftchildren->Count > i ? leftchildren[i] : nullptr,
			rightchildren->Count > i ? rightchildren[i] : nullptr,
			differences_only,
			prefix);
	}
	return 0;
}

int compare(ResourceNode^ left, ResourceNode^ right, bool differences_only) {
	return compare(left, right, differences_only, "");
}

int compare(array<String^>^ args) {
	bool differences_only = false;
	ResourceNode^ file1;
	ResourceNode^ file2;
	for each(String^ argument in args) {
		if (argument == "--help" || argument == "/?") {
			Console::Write(gcnew String(brawlsdiff_usage));
			return 0;
		} else if (argument == "sdiff") {
			continue;
		} else if (argument == "-s") {
			differences_only = true;
		} else if (file1 == nullptr) {
			if (!File::Exists(argument)) {
				Console::Error->WriteLine("File does not exist: " + argument);
				return 1;
			}
			file1 = NodeFactory::FromFile(nullptr, argument);
		} else if (file2 == nullptr) {
			if (!File::Exists(argument)) {
				Console::Error->WriteLine("File does not exist: " + argument);
				return 1;
			}
			file2 = NodeFactory::FromFile(nullptr, argument);
		} else {
			Console::Error->WriteLine("Too many arguments: " + argument);
			return 1;
		}
	}
	if (file1 == nullptr) {
		Console::Error->WriteLine("No file defined");
		return 1;
	}
	return compare(file1, file2, differences_only);
}
