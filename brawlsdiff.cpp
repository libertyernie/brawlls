#include "brawlprintf.h"
#include "brawlsdiff.h"
#include "isinst.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::IO;
using namespace BrawlLib::SSBB::ResourceNodes;

int compare(array<String^>^ args) {
	ResourceNode^ file1;
	ResourceNode^ file2;
	for each(String^ argument in args) {
		if (argument == "sdiff") {
			continue;
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
	return compare(file1, file2);
}

int compare(ResourceNode^ left, ResourceNode^ right) {
	return compare(left, right, "");
}

const int colwidth = 78;

int compare(ResourceNode^ left, ResourceNode^ right, String^ prefix) {
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
	if (true||sepchar != ' ') Console::WriteLine(%line);

	prefix += "  ";

	//if (isinst<MDL0Node^>(left)) left = nullptr;
	//if (isinst<MDL0Node^>(right)) right = nullptr;

	List<ResourceNode^>^ leftchildren = (left == nullptr) ? gcnew List<ResourceNode^>() : left->Children;
	List<ResourceNode^>^ rightchildren = (right == nullptr) ? gcnew List<ResourceNode^>() : right->Children;
	int maxct = Math::Max(leftchildren->Count, rightchildren->Count);
	for (int i = 0; i < maxct; i++) {
		int result = compare(
			leftchildren->Count > i ? leftchildren[i] : nullptr,
			rightchildren->Count > i ? rightchildren[i] : nullptr,
			prefix);
	}
	return 0;
}