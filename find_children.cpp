#include "find_children.h"

using BrawlLib::SSBB::ResourceNodes::ResourceNode;
using namespace System;
using System::Collections::Generic::List;
using System::Text::RegularExpressions::Regex;

void find_children(ResourceNode^ root, String^ nodepath, List<ResourceNode^>^ output, bool useBrawlLibSearchChildren) {
	if (nodepath == nullptr) {
		output->Add(root);
	} else if (useBrawlLibSearchChildren) {
		output->Add(root->FindChild(nodepath, true));
	} else {
		find_children_recursive(root, nodepath, output);
	}
}

void find_children_recursive(ResourceNode^ root, String^ nodepath, List<ResourceNode^>^ output) {
	nodepath = nodepath->Replace("+", "/+")->Replace("//", "/");
	while (nodepath->StartsWith("/")) {
		nodepath = nodepath->Substring(1);
	}

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